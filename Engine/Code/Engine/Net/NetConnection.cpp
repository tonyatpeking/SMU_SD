#include "Engine/Math/MathUtils.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetMessageDefinition.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Net/EngineNetMessages.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Net/PacketTracker.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Net/NetMessageDatabase.hpp"

NetConnection::NetConnection()
{
    m_sendTimer = new Timer( Clock::GetRealTimeClock(), 1.f / m_sendRate );
    m_heartbeatTimer = new Timer( Clock::GetRealTimeClock(), 1.f / m_heartbeatRate );

    m_packetTrackers.resize( PACKET_TRACKER_COUNT );
    for( auto& packetTracker : m_packetTrackers )
    {
        packetTracker = new PacketTracker();
    }
    m_packetLossTracker.resize( PACKET_TRACKER_COUNT, 1 );
}

NetConnection::~NetConnection()
{
    ContainerUtils::DeletePointers( m_packetTrackers );

    delete m_heartbeatTimer;
    delete m_sendTimer;
}

void NetConnection::ProcessOutgoing()
{
    if( !PopSendTimer() )
        return;

    if( PopHeartbeatTimer() )
    {
        QueueSend( EngineNetMessages::Compose_Heartbeat() );
    }

    NetPacket packet;
    packet.m_receiver = this;
    while( !m_outboundUnreliables.empty() || m_shouldForceSend )
    {
        packet.SetWriteHead( sizeof( PacketHeader ) );
        PacketHeader& header = packet.m_header;
        header.m_senderConnectionIdx = m_owningSession->GetMyConnectionIdx();

        header.m_lastReceivedAck = m_lastReceivedAck;
        header.m_receivedAckBitfield = m_receivedAckBitfield;
        header.m_unreliableCount = 0;

        FillPacketWithUnreliables( packet );

        // TODO: possibly send another packet, or discard leftover unreliables

        if( header.m_unreliableCount != 0 /*TODO: ||  m_reliableCount !=0*/ )
        {
            header.m_ack = m_nextAckToSend;
            AddPacketTracker( m_nextAckToSend );
            ++m_nextAckToSend;
        }
        else
        {
            header.m_ack = INVALID_PACKET_ACK;
        }

        packet.PackHeader();
        SendImmediate( packet );
        m_shouldForceSend = false;
    }
}

bool NetConnection::OnReceivePacket( NetPacket& packet, bool processSuccess )
{
    UNUSED( processSuccess );

    m_timeOfLastReceiveMS = TimeUtils::GetCurrentTimeMS();

    PacketHeader& header = packet.m_header;

    if( header.m_ack != INVALID_PACKET_ACK )
    {
        UpdateLastReceivedAck( header.m_ack );
    }

    if( header.m_lastReceivedAck != INVALID_PACKET_ACK )
    {
        ConfirmPacketsReceivedByOther( header.m_lastReceivedAck, header.m_receivedAckBitfield );
    }

    return true;
}

void NetConnection::QueueSend( NetMessage* netMsg )
{
    auto def = netMsg->m_def;
    if( def == nullptr )
    {
        LOG_ERROR_TAG( "Net", "Could not send message, no definition for %s",
                       netMsg->m_name.c_str() );
        return;
    }
    m_outboundUnreliables.push( netMsg );
}

void NetConnection::SendImmediate( const NetPacket& packet )
{
    m_timeOfLastSendMS = TimeUtils::GetCurrentTimeMS();

    m_owningSession->SendImmediate( packet );
}

void NetConnection::Close()
{
    SetClosed( true );
}

bool NetConnection::IsValid()
{
    return !IsClosed() && !m_address.IsInvalid();
}

void NetConnection::FillPacketWithUnreliables( NetPacket& packet )
{
    while( !m_outboundUnreliables.empty() )
    {
        NetMessage* msg = m_outboundUnreliables.front();
        if( packet.WriteMessage( *msg ) )
        {
            delete msg;
            m_outboundUnreliables.pop();
            ++packet.m_header.m_unreliableCount;
            ASSERT_OR_DIE( packet.m_header.m_unreliableCount != 0U,
                           "exceeded 256 messages!" );
        }
        // packet is full
        else
        {
            return;
        }
    }
}

void NetConnection::SetSendRate( float hz )
{
    m_sendRate = hz;
}

float NetConnection::GetEffectiveSendInterval() const
{
    return 1.f / Min( m_sendRate, m_owningSession->m_sendRate );
}

bool NetConnection::PopSendTimer()
{
    if( GetEffectiveSendInterval() != m_sendTimer->m_lapTime )
        m_sendTimer->SetLapTime( GetEffectiveSendInterval() );

    if( m_sendTimer->PopOneLap() )
    {
        m_sendTimer->Reset();
        return true;
    }
    return false;
}

bool NetConnection::PopHeartbeatTimer()
{
    if( 1.f / m_heartbeatRate != m_heartbeatTimer->m_lapTime )
        m_heartbeatTimer->SetLapTime( 1.f / m_heartbeatRate );

    if( m_heartbeatTimer->PopOneLap() )
    {
        m_heartbeatTimer->Reset();
        return true;
    }
    return false;
}

void NetConnection::UpdateLastReceivedAck( uint16 ackFromOther )
{
    ++m_receivedAcksCount;
    // this will force a send to confirm the ack
    m_shouldForceSend = true;
    // this automatically works for uint16 wrap around
    uint16 dist = ackFromOther - m_lastReceivedAck;

    // dist is positive
    if( ( dist & 0x8000 ) == 0 )
    {
        m_lastReceivedAck = ackFromOther;

        m_receivedAckBitfield <<= dist;
        m_receivedAckBitfield |= ( 1 << ( dist - 1 ) ); // set the old highests bit;
    }
    // dist is negative
    else
    {
        // got an older ack than highest
        dist = m_lastReceivedAck - ackFromOther; // dist from highest
        m_receivedAckBitfield |= ( 1 << ( dist - 1 ) );  // set bit in history

        // may want to check that bit WAS zero, otherwise you double processed a packet - bad!
    }
}


void NetConnection::ConfirmPacketsReceivedByOther( uint16 ackISentBefore, uint16 ackBitfield )
{
    ConfirmOnePacketReceivedByOther( ackISentBefore );

    for( uint16 i = 0; i < 16; ++i )
    {
        // this will automatically skip INVALID_PACKET_ACK because the bit for it
        // will never be set
        uint16 bitMask = 1 << i;
        // if that bit is set
        if( ackBitfield & bitMask )
        {
            ConfirmOnePacketReceivedByOther( ackISentBefore - ( i + 1U ) );
        }
    }
}

void NetConnection::ConfirmOnePacketReceivedByOther( uint16 ackISentBefore )
{
    if( ackISentBefore == INVALID_PACKET_ACK )
        return;
    PacketTracker *tracker = GetPackerTracker( ackISentBefore );
    if( nullptr == tracker )
        return;


    // Update Round trip time
    uint rttLatest = TimeUtils::GetCurrentTimeMS() - tracker->m_timeOfSendMS;
    m_roundTripTime = (uint) Lerp( (float) rttLatest, (float) m_roundTripTime, m_roundTripTimeInertia );
    tracker->m_ack = INVALID_PACKET_ACK;
}

PacketTracker* NetConnection::AddPacketTracker( uint16 ackIJustSent )
{
    PacketTracker* tracker = m_packetTrackers[m_nextFreePacketTrackerSlot];

    // we lost a packer if we are overwriting a valid packet ack
    bool notOverwriting = ( tracker->m_ack == INVALID_PACKET_ACK );
    UpdateLossTracker( notOverwriting );

    tracker->m_ack = ackIJustSent;
    tracker->m_timeOfSendMS = TimeUtils::GetCurrentTimeMS();

    ++m_nextFreePacketTrackerSlot;

    if( m_nextFreePacketTrackerSlot == PACKET_TRACKER_COUNT )
        m_nextFreePacketTrackerSlot = 0;

    return tracker;
}

PacketTracker* NetConnection::GetPackerTracker( uint16 ackISentBefore )
{
    for( auto& tracker : m_packetTrackers )
    {
        if( tracker->m_ack == ackISentBefore )
            return tracker;
    }
    return nullptr;
}

void NetConnection::UpdateLossTracker( bool packetWasReceived )
{
    m_packetLossTracker[m_nextFreeLossTrackerSlot] = packetWasReceived ? 1 : 0;
    ++m_nextFreeLossTrackerSlot;

    if( m_nextFreeLossTrackerSlot == PACKET_TRACKER_COUNT )
        m_nextFreeLossTrackerSlot = 0;
}

float NetConnection::CalculateLossRate()
{
    int totalNotLost = 0;
    for( auto& wasNotLost : m_packetLossTracker )
    {
        totalNotLost += wasNotLost;
    }
    return 1.f - ( (float) totalNotLost / (float) PACKET_TRACKER_COUNT );
}

