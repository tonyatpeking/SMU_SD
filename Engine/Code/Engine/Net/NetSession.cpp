#include "Engine/Time/Time.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetPacket.hpp"
#include "Engine/DataUtils/BytePacker.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/UDPSocket.hpp"
#include "Engine/Net/NetCommand.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Net/PacketChannel.hpp"
#include "Engine/Net/NetMessageDefinition.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetMessageDatabase.hpp"

#include <algorithm>

namespace
{
NetSession* s_defaultSession = nullptr;
}

NetSession* NetSession::GetDefault()
{
    if( s_defaultSession == nullptr )
        s_defaultSession = new NetSession();
    return s_defaultSession;
}

NetSession::NetSession()
{

}

NetSession::~NetSession()
{
}

void NetSession::BindAndFinalize( int port, uint rangeToTry /*= 0U */ )
{
    m_packetChannel = new PacketChannel();
    m_packetChannel->m_owningSession = this;
    bool success = false;

    for( uint portOffset = 0; portOffset <= rangeToTry; ++portOffset )
    {
        NetAddress localAddr = NetAddress::GetLocal( ToString( port + portOffset ) );
        success = m_packetChannel->Bind( localAddr );

        if( success )
            break;
    }

    if( !success )
    {
        LOG_ERROR_TAG( "Net", "Failed to bind port(s): [%d], range:[%d]",
                       port, rangeToTry );
    }

    NetMessageDatabase::Finalize();
}

NetConnection* NetSession::AddConnection( uint8 idx, const NetAddress& addr )
{
    if( ContainerUtils::ContainsKey( m_connections, idx ) )
    {
        LOG_WARNING_TAG(
            "Net", "Connection index [%d] already exists, replacing", idx );

        delete m_connections[idx];
    }
    LOG_INFO_TAG( "Net", "Connection added at index [%d]", idx );
    NetConnection* connection = new NetConnection();
    connection->m_address = addr;
    connection->m_owningSession = this;
    connection->m_idxInSession = idx;
    connection->m_isClosed = false;
    m_connections[idx] = connection;

    if( addr == GetMyAddress() )
        m_myConnectionIdx = idx;

    return connection;
}

NetConnection* NetSession::AddConnectionSelf( uint8 idx )
{
    return AddConnection( idx, GetMyAddress() );
}

NetConnection* NetSession::GetConnection( uint8 idx )
{
    if( !ContainerUtils::ContainsKey( m_connections, idx ) )
    {
        LOG_WARNING_TAG( "Net", "Connection index [%d] does not exist", idx );
        return nullptr;
    }
    return m_connections[idx];
}

NetConnection* NetSession::GetConnection( const NetAddress& addr )
{
    for( auto& pair : m_connections )
    {
        if( pair.second->m_address == addr )
            return pair.second;
    }
    return nullptr;
}

void NetSession::CloseAllConnections()
{
    for( auto& pair : m_connections )
    {
        pair.second->Close();
    }
}

NetConnection* NetSession::GetMyConnection()
{
    return m_connections[m_myConnectionIdx];
}

NetAddress NetSession::GetMyAddress()
{
    return m_packetChannel->m_socket->m_address;
}

void NetSession::ProcessIncomming()
{
#ifdef SIM_NET_LATENCY
    ProcessIncommingWithLatency();
#else
    ProcessIncommingImmediate();
#endif // SIM_NET_LATENCY
}

void NetSession::ProcessIncommingWithLatency()
{
    Random& rnd = *Random::Default();
    NetPacket* packet = new NetPacket();
    while( m_packetChannel->Receive( *packet ) )
    {
        if( ShouldDiscardPacketForLossSim() )
            continue;

        packet->m_timeOfReceiveMS = TimeUtils::GetCurrentTimeMS()
            + (uint) rnd.IntInRange( m_minSimLatencyMS, m_maxSimLatencyMS );
        m_queuedPacketsToProcess.push( packet );
        packet = new NetPacket();
    }
    delete packet;

    while( !m_queuedPacketsToProcess.empty() )
    {
        NetPacket* packetToProcess = m_queuedPacketsToProcess.top();
        if( packetToProcess->m_timeOfReceiveMS < TimeUtils::GetCurrentTimeMS() )
        {
            bool processSuccess = ProcessPacket( *packetToProcess );
            if( !processSuccess )
            {
                // bad packet, update connection with this info
                LOG_WARNING_TAG(
                    "Net", "Bad Packet from %s",
                    packetToProcess->m_senderAddress.ToStringAll().c_str() );
                LOG_WARNING_TAG(
                    "Net", "Bad Packet Data: %s",
                    packetToProcess->ToString().c_str() );
            }
            NetConnection* connection = packetToProcess->m_sender;
            if( connection )
                connection->OnReceivePacket( *packetToProcess, processSuccess );

            delete packetToProcess;
            m_queuedPacketsToProcess.pop();
        }
        else
        {
            break;
        }
    }
}

void NetSession::ProcessIncommingImmediate()
{
    NetPacket packet;
    while( m_packetChannel->Receive( packet ) )
    {
        if( ShouldDiscardPacketForLossSim() )
            continue;

        bool processSuccess = ProcessPacket( packet );
        if( !processSuccess )
        {
            // bad packet, update connection with this info
            LOG_WARNING_TAG( "Net", "Bad Packet from %s",
                             packet.m_senderAddress.ToStringAll().c_str() );
            LOG_WARNING_TAG( "Net", "Bad Packet Data: %s",
                             packet.ToString().c_str() );
        }
        NetConnection* connection = packet.m_sender;
        if( connection )
            connection->OnReceivePacket( packet, processSuccess );

    }
}

bool NetSession::ProcessPacket( NetPacket& packet )
{
    if( LOG_PACKET_BYTES )
    {
        LOG_INFO_TAG(
            "Net",
            "Packet: %s",
            packet.ToString().c_str() );
    }

    if( VALIDATE_PACKET && !packet.IsValid() )
        return false;
    packet.SetReadHead( sizeof( PacketHeader ) );
    NetMessage msg;
    for( int msgIdx = 0; msgIdx < packet.m_header.m_unreliableCount; ++msgIdx )
    {
        if( !packet.ExtractMessage( msg ) )
            return false;

        if( !ProcessMessage( msg ) )
            return false;
    }
    return true;
}

bool NetSession::ProcessMessage( NetMessage& message )
{
    const NetMessageDefinition* def = message.m_def;
    MessageID msgID = message.GetMessageID();
    if( !def )
    {
        LOG_WARNING_TAG( "Net",
                         "No definition found for messageID [%d]",
                         msgID );
        return false;
    }

    if( def->RequiresConnection() && message.m_sender == nullptr )
    {
        LOG_WARNING_TAG( "Net",
                         "MessageID [%d] required connection when there was none"
                         , msgID );
        return false;
    }

    if( !def->m_callback )
    {
        LOG_WARNING_TAG( "Net", "MessageID [%d] has no callback", msgID );
        return false;
    }

    if( !def->m_callback( &message ) )
    {
        LOG_WARNING_TAG( "Net", "MessageID [%d] callback failed", msgID );
        return false;
    }

    if( message.GetReadableByteCount() != 0 )
    {
        LOG_WARNING_TAG(
            "Net",
            "[%s] Message [%s] ran successfully but did not use all bytes",
            message.m_senderAddress.ToStringAll().c_str(),
            def->m_name.c_str() );
    }
    return true;
}

void NetSession::ProcessOutgoing()
{
    for( auto& pair : m_connections )
    {
        if( pair.second->IsClosed() )
            continue;

        pair.second->ProcessOutgoing();
    }
}

void NetSession::SendImmediate( const NetPacket& packet )
{
    m_packetChannel->SendImmediate( packet );
}

bool NetSession::SendToConnection( uint8 idx, NetMessage* message )
{
    NetConnection* connection = GetConnection( idx );
    if( !connection )
    {
        LOG_ERROR_TAG(
            "Net",
            "Could not send to connection, connection [%d] does not exist",
            idx );
        return false;
    }
    connection->QueueSend( message );
    return true;
}

void NetSession::SetSimLoss( float lossAmount )
{
    m_simLossAmount = lossAmount;
}

void NetSession::SetSimLatency( uint minSimLatencyMS, uint maxSimLatencyMS /*= 0U */ )
{
    m_minSimLatencyMS = minSimLatencyMS;
    m_maxSimLatencyMS = Max( minSimLatencyMS, maxSimLatencyMS );
}

bool NetSession::ShouldDiscardPacketForLossSim() const
{
#ifdef SIM_NET_LOSS
    return Random::Default()->CheckChance( m_simLossAmount );
#else
    return false;
#endif // SIM_NET_LOSS
}

void NetSession::SetSessionSendRate( float Hz )
{
    m_sendRate = Hz;
}

void NetSession::SetConnectionSendRate( uint8 connectionIdx, float Hz )
{
    NetConnection* connection = GetConnection( connectionIdx );
    if( connection )
        connection->SetSendRate( Hz );
}

void NetSession::SetHeartBeat( float Hz )
{
    for( auto& pair : m_connections )
    {
        pair.second->m_heartbeatRate = Hz;
    }
}


bool NetSession::GreaterThanByTime::operator()(
    const NetPacket* lhs, const NetPacket* rhs ) const
{
    return lhs->m_timeOfReceiveMS > rhs->m_timeOfReceiveMS;
}
