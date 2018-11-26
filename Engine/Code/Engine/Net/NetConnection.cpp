
#include "Engine/Net/NetCommonC.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Time/Time.hpp"

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

    for( int i = 0; i < MAX_MESSAGE_CHANNELS; ++i )
    {
        m_messageChannels[i] = new NetMessageChannel();
    }
}

NetConnection::~NetConnection()
{
    ContainerUtils::DeletePointers( m_packetTrackers );

    delete m_heartbeatTimer;
    delete m_sendTimer;

    for( int i = 0; i < MAX_MESSAGE_CHANNELS; ++i )
    {
        delete m_messageChannels[i];
    }
}

void NetConnection::Flush()
{
    if( !PopSendTimer() )
        return;

    if( PopHeartbeatTimer() )
    {
        QueueSend( EngineNetMessages::Compose_Heartbeat() );
    }

    NetPacket packet;
    packet.m_receiver = this;
    packet.m_receiverAddress = this->m_address;
    if( !m_unsentUnreliables.empty()
        || !m_unconfirmedReliables.empty()
        || !m_unsentReliables.empty()
        || m_shouldForceSend )
    {
        packet.SetWriteHead( sizeof( PacketHeader ) );
        PacketHeader& header = packet.m_header;
        header.m_senderConnectionIdx = m_owningSession->GetMyConnectionIdx();

        header.m_lastReceivedAck = m_lastReceivedAck;
        header.m_receivedAckBitfield = m_receivedAckBitfield;
        header.m_message_count = 0;


        FillPacketWithUnconfirmedReliables( packet );
        FillPacketWithUnsentReliables( packet );
        FillPacketWithUnsentUnreliables( packet );
        ClearUnreliables();

        if( header.m_message_count != 0 /*TODO: ||  m_reliableCount !=0*/ )
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

    if( def->IsInOrder() )
    {
        NetMessageChannel* channel = m_messageChannels[def->GetChannelIdx()];
        netMsg->m_sequenceID = channel->m_nextSequenceIDToSend;
        ++( channel->m_nextSequenceIDToSend );
    }

    if( def->IsReliable() )
        m_unsentReliables.push( netMsg );
    else
        m_unsentUnreliables.push( netMsg );
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

void NetConnection::FillPacketWithUnsentUnreliables( NetPacket& packet )
{
    while( !m_unsentUnreliables.empty() )
    {
        NetMessage* msg = m_unsentUnreliables.front();
        if( packet.WriteMessage( *msg ) )
        {
            delete msg;
            m_unsentUnreliables.pop();
        }
        // packet is full
        else
        {
            return;
        }
    }
}

void NetConnection::FillPacketWithUnconfirmedReliables( NetPacket& packet )
{
    float currentTime = TimeUtils::GetCurrentTimeSecondsF();
    for( auto& msg : m_unconfirmedReliables )
    {
        if( currentTime - msg->m_lastSentTime > GetReliableResendWait() )
        {
            if( packet.WriteMessage( *msg ) )
            {
                msg->m_lastSentTime = currentTime;

                GetCurrentPacketTracker()->AddReliable( msg->m_reliableID );
                LOG_INFO_TAG( "Debug", "resend" );
            }
            else
            {
                return;
            }
        }
    }
}

void NetConnection::FillPacketWithUnsentReliables( NetPacket& packet )
{
    float currentTime = TimeUtils::GetCurrentTimeSecondsF();
    while( !m_unsentReliables.empty() )
    {
        // reached window limit
        if( !CanSendNewReliable() )
            return;
        NetMessage* msg = m_unsentReliables.front();
        msg->m_reliableID = m_nextReliableID;
        msg->m_lastSentTime = currentTime;

        if( packet.WriteMessage( *msg ) )
        {
            ++m_nextReliableID;
            m_unconfirmedReliables.push_back( msg );
            m_unsentReliables.pop();

            GetCurrentPacketTracker()->AddReliable( msg->m_reliableID );
        }
        // packet is full
        else
        {
            return;
        }
    }
}

void NetConnection::ClearUnreliables()
{
    while( !m_unsentUnreliables.empty() )
    {
        delete m_unsentUnreliables.front();
        m_unsentUnreliables.pop();
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

    // Reliables
    for( int idx = 0; idx < (int) tracker->m_reliablesInPacket; ++idx )
    {
        uint16 reliableID = tracker->m_sentReliableIds[idx];
        ConfirmReliable( reliableID );
    }


    tracker->Invalidate();
}

PacketTracker* NetConnection::GetCurrentPacketTracker()
{
    return m_packetTrackers[m_nextFreePacketTrackerSlot];
}

PacketTracker* NetConnection::AddPacketTracker( uint16 ackIJustSent )
{
    PacketTracker* tracker = GetCurrentPacketTracker();

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

float NetConnection::GetReliableResendWait()
{
    float wait = RELIABLE_RESEND_WAIT_MULTIPLIER * m_roundTripTime;
    wait = Clampf( wait, RELIABLE_RESEND_WAIT_MIN, RELIABLE_RESEND_WAIT_MAX );
    return RELIABLE_RESEND_WAIT_FIXED;
}

void NetConnection::ConfirmReliable( uint16 reliableID )
{
    for( int i = (int) m_unconfirmedReliables.size() - 1; i >= 0; --i )
    {
        NetMessage* msg = m_unconfirmedReliables[i];
        if( msg->m_reliableID == reliableID )
        {
            delete msg;
            ContainerUtils::EraseAtIndexFast( m_unconfirmedReliables, i );
            return;
        }

    }
}

bool NetConnection::HasReliableBeenProcessed( uint16 reliableID )
{
    bool hasBeenProcessed = ContainerUtils::Contains(
        m_receivedReliableIDs, reliableID );
    uint16 diff = m_highestReceivedReliabeID - RELIABLE_WINDOW + 1;
    bool outOfTrackingWindow = CyclicLesser( reliableID, diff );

    return outOfTrackingWindow || hasBeenProcessed;
}

void NetConnection::MarkReliableProcessed( uint16 reliableID )
{
    if( CyclicLesser( m_highestReceivedReliabeID, reliableID ) )
    {
        m_highestReceivedReliabeID = reliableID;
        for( int i = (int) m_receivedReliableIDs.size() - 1; i >= 0; --i )
        {
            uint16 msgID = m_receivedReliableIDs[i];
            uint16 diff = m_highestReceivedReliabeID - msgID;
            if( diff >= RELIABLE_WINDOW )
            {
                ContainerUtils::EraseAtIndexFast( m_receivedReliableIDs, i );
            }
        }
    }
    m_receivedReliableIDs.push_back( reliableID );
}

uint16 NetConnection::GetOldestUnconfirmedReliableID()
{
    uint16 lowestID = m_unconfirmedReliables[0]->m_reliableID;

    for( auto& msg : m_unconfirmedReliables )
    {
        uint16 currentID = msg->m_reliableID;
        if( CyclicLesser( currentID, lowestID ) )
        {
            lowestID = currentID;
        }
    }
    return lowestID;
}

bool NetConnection::CanSendNewReliable()
{
    if( m_unconfirmedReliables.empty() )
        return true;

    uint16 lowestID = GetOldestUnconfirmedReliableID();
    // dist auto wraps around for uint16
    uint16 dist = m_nextReliableID - lowestID;
    return dist < RELIABLE_WINDOW;
}

bool NetConnection::IsMe() const
{
    return m_owningSession->m_myConnection == this;
}

bool NetConnection::IsHost() const
{
    return m_owningSession->m_hostConnection == this;
}

bool NetConnection::IsClient() const
{
    return !IsHost();
}

bool NetConnection::IsConnected() const
{
    return m_state == eConnectionState::CONNECTED
        || m_state == eConnectionState::READY;
}

bool NetConnection::IsDisconnected() const
{
    return m_state == eConnectionState::DISCONNECTED;
}

bool NetConnection::IsReady() const
{
    return m_state == eConnectionState::READY;
}

void NetConnection::SetState( eConnectionState state )
{
    m_state = state;
    if( IsMe() )
    {
        for( auto& connection : m_owningSession->m_connections )
        {
            if( connection.second != this )
            {
                NetMessage* updateState =
                    EngineNetMessages::Compose_UpdateConnectionState( state );
                m_owningSession->SendToConnection( connection.first, updateState );
            }
        }
    }
}

