#include "Engine/Time/Time.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Math/Random.hpp"

#include "Engine/Net/NetCommonC.hpp"

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
    m_netClock = new Clock();
}

NetSession::~NetSession()
{
    Disconnect();
    delete m_netClock;
}

void NetSession::Host( const string& myID, int port, uint rangeToTry /*= 0U */ )
{
    UNUSED( myID );
    if( m_state != eSessionState::DISCONNECTED )
    {
        LOG_WARNING_TAG( "Net", "Cannot host, not in disconnected state" );
        return;
    }

    if( !BindToPort( port, rangeToTry ) )
        return;

    NetConnection* connection = CreateConnection( m_boundAddress );
    BindConnection( 0, connection );
    m_myConnection = connection;
    m_myConnectionIdx = 0;
    m_hostConnection = connection;
    connection->m_state = eConnectionState::READY;
    m_state = eSessionState::READY;
    m_shouldResetClock = true;
}

void NetSession::Join( const string& myID, const NetAddress& hostAddr )
{
    UNUSED( myID );
    if( m_state != eSessionState::DISCONNECTED )
    {
        LOG_WARNING_TAG( "Net", "Cannot join, not in disconnected state" );
        return;
    }

    if( !BindToPort( hostAddr.m_port, MAX_CONNECTION_COUNT ) )
        return;

    // host connection
    NetConnection* hostConnection = CreateConnection( hostAddr );
    BindConnection( 0, hostConnection );
    m_hostConnection = hostConnection;
    hostConnection->m_state = eConnectionState::CONNECTED;

    // my connection
    m_myConnection = CreateConnection( m_boundAddress );
    m_myConnection->m_state = eConnectionState::CONNECTING;

    m_state = eSessionState::CONNECTING;
    ResetJoinTimeout();
}

void NetSession::Disconnect()
{
    SendHangupToAll();

    // these are deleted in the list so do not delete here
    m_hostConnection = nullptr;
    m_myConnection = nullptr;

    for( auto& connection : m_unboundConnections )
        delete connection;
    m_unboundConnections.clear();

    for( auto& pair : m_connections )
        delete pair.second;
    m_connections.clear();

    m_packetChannel->Close();
    m_state = eSessionState::DISCONNECTED;
    m_lastReceivedHostTime = 0.f;
}

void NetSession::ShouldDisconnect()
{
    m_state = eSessionState::SHOULD_DISCONNECT;
}

bool NetSession::IsHost()
{
    return ( m_myConnection != nullptr ) && m_myConnection->IsHost();
}

void NetSession::SendJoinRequestWithInterval()
{
    static Timer* timer = new Timer( Clock::GetRealTimeClock(),
                                     JOIN_REQUEST_RESEND_TIME );
    if( timer->PopAllLaps() != 0 )
    {
        NetMessage* request = EngineNetMessages::Compose_JoinRequest();
        SendImmediateConnectionless( request, m_hostConnection->m_address );
    }
}


void NetSession::ResetJoinTimeout()
{
    delete m_joinTimeoutTimer;
    m_joinTimeoutTimer = new Timer( Clock::GetRealTimeClock(), JOIN_TIMEOUT );
}

bool NetSession::DidJoinTimeout()
{
    return m_joinTimeoutTimer->PeekOneLap();
}

void NetSession::SendHangupToAll()
{
    for( auto& pair : m_connections )
    {
        NetMessage* msg = EngineNetMessages::Compose_Hangup();
        uint8 idx = pair.first;
        SendToConnection( idx, msg );
    }
    Flush( true );
}

bool NetSession::ProcessJoinRequest( NetMessage* msg )
{
    NetAddress senderAddr = msg->m_senderAddress;
    uint8 idx = GetAvailableConnectionIdx();

    if( !IsHost() || ReachedMaxClients()
        || ( idx == INVALID_CONNECTION_INDEX ) )
    {
        NetMessage* denyMsg = EngineNetMessages::Compose_JoinDeny();
        SendImmediateConnectionless( denyMsg, senderAddr );
        return false;
    }

    if( GetConnection( senderAddr ) != nullptr )
        return true;

    NetConnection* newConnection = CreateConnection( senderAddr );
    BindConnection( idx, newConnection );
    NetMessage* acceptMsg = EngineNetMessages::Compose_JoinAccept( idx );
    SendToConnection( idx, acceptMsg );
    NetMessage* finishedMsg = EngineNetMessages::Compose_JoinFinished();
    SendToConnection( idx, finishedMsg );
    return true;
}

bool NetSession::ProcessJoinDeny( NetMessage* msg )
{
    UNUSED( msg );
    SetError( eSessionError::JOIN_DENIED );
    return true;
}

bool NetSession::ProcessJoinAccept( uint8 assignedIdx )
{
    BindConnection( assignedIdx, m_myConnection );
    m_myConnection->SetState( eConnectionState::CONNECTED );
    m_shouldResetClock = true;
    return true;
}

bool NetSession::ProcessNewConnection( NetMessage* msg )
{
    UNUSED( msg );
    return true;
}

bool NetSession::ProcessJoinFinished( NetMessage* msg )
{
    UNUSED( msg );
    return true;
}

bool NetSession::ProcessUpdateConnectionState( NetMessage* msg,
                                               eConnectionState state )
{
    NetConnection* connection = GetConnection( msg->m_senderIdx );
    if( connection )
        connection->m_state = state;
    return true;
}

bool NetSession::ProcessHangup( NetMessage* msg )
{
    LOG_INFO_TAG( "Net", "Graceful hangup" );
    uint8 connectionIdx = msg->m_senderIdx;
    if( ContainerUtils::ContainsKey( m_connections, connectionIdx ) )
    {
        delete m_connections[connectionIdx];
        m_connections.erase( connectionIdx );
    }
    return true;
}

bool NetSession::ProcessHeartbeat( NetMessage* msg, uint hostTimeMS )
{
    if( IsHost() )
        return true;
    NetConnection* connection = GetConnection( msg->m_senderIdx );
    if( connection->IsHost() )
    {
        NetConnection* host = connection;
        float newHostTime =
            ( (float) hostTimeMS ) / 1000.f + host->m_roundTripTime / 2.f;

        m_lastReceivedHostTime = Maxf( m_lastReceivedHostTime, newHostTime );

    }
    return true;
}

bool NetSession::BindToPort( int port, uint rangeToTry /*= 0U */ )
{
    bool success = false;
    NetAddress localAddr;

    for( uint portOffset = 0; portOffset <= rangeToTry; ++portOffset )
    {
        localAddr = NetAddress::GetLocal( ToString( port + portOffset ) );
        success = m_packetChannel->Bind( localAddr );

        if( success )
        {
            m_boundAddress = localAddr;
            return true;
        }
    }

    LOG_ERROR_TAG( "Net", "Failed to bind port(s): [%d], range:[%d]",
                   port, rangeToTry );
    SetError( eSessionError::FAILED_TO_BIND_PORT );
    return false;
}

void NetSession::Finalize()
{
    m_packetChannel = new PacketChannel();
    m_packetChannel->m_owningSession = this;

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


NetConnection* NetSession::CreateConnection( const NetAddress& addr )
{
    NetConnection* connection = new NetConnection();
    connection->m_address = addr;
    connection->m_owningSession = this;
    connection->m_isClosed = false;
    m_unboundConnections.push_back( connection );
    return connection;
}

void NetSession::DestroyConnection( NetConnection* connection )
{
    ContainerUtils::EraseOneValue( m_unboundConnections, connection );
    ContainerUtils::EraseOneValue( m_connections, connection );
    if( m_myConnection == connection )
        m_myConnection = nullptr;
    if( m_hostConnection == connection )
        m_hostConnection = nullptr;
    delete connection;
}

void NetSession::BindConnection( uint8 idx, NetConnection* connection )
{
    if( ContainerUtils::ContainsKey( m_connections, idx ) )
    {
        LOG_WARNING_TAG(
            "Net", "Connection index [%d] already exists, replacing", idx );

        delete m_connections[idx];
    }
    LOG_INFO_TAG( "Net", "Connection bound at index [%d]", idx );

    m_connections[idx] = connection;
    connection->m_idxInSession = idx;
    ContainerUtils::EraseOneValue( m_unboundConnections, connection );

    if( connection == m_myConnection )
        m_myConnectionIdx = idx;
}

void NetSession::Update()
{
    if( m_state == eSessionState::SHOULD_DISCONNECT )
    {
        Disconnect();
        return;
    }

    if( m_state == eSessionState::DISCONNECTED )
        return;

    if( m_state == eSessionState::CONNECTING )
    {
        if( m_myConnection->m_state == eConnectionState::CONNECTED )
        {
            m_state = eSessionState::JOINING;
            m_myConnection->m_state = eConnectionState::READY;
        }
        SendJoinRequestWithInterval();
        if( DidJoinTimeout() )
        {
            LOG_ERROR_TAG( "Net", "Join Timed out" );
            ShouldDisconnect();
        }
    }
    if( m_state == eSessionState::JOINING )
    {
        if( m_myConnection->m_state == eConnectionState::READY )
            m_state = eSessionState::READY;
    }

#ifdef SIM_NET_LATENCY
    ProcessIncommingWithLatency();
#else
    ProcessIncommingImmediate();
#endif // SIM_NET_LATENCY

    UpdateNetClock();

    CheckForTimeoutConnections();


}

void NetSession::ProcessIncommingWithLatency()
{
    Random& rnd = *Random::Default();
    NetPacket* packet = new NetPacket();
    while( m_packetChannel->Receive( *packet ) )
    {
        if( ShouldDiscardPacketForLossSim() )
            continue;

        packet->m_timeOfReceive = TimeUtils::GetCurrentTimeSecondsF()
            + rnd.FloatInRange( m_minSimLatency, m_maxSimLatency );
        m_queuedPacketsToProcess.push( packet );
        packet = new NetPacket();
    }
    delete packet;

    while( !m_queuedPacketsToProcess.empty() )
    {
        NetPacket* packetToProcess = m_queuedPacketsToProcess.top();
        if( packetToProcess->m_timeOfReceive < TimeUtils::GetCurrentTimeSecondsF() )
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
            NetConnection* connection =
                GetConnection( packetToProcess->m_senderIdx );
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
        NetConnection* connection = GetConnection( packet.m_senderIdx );
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
    {
        LOG_WARNING_TAG(
            "Net",
            "Packet Validation Failed!" );
        return false;
    }
    packet.SetReadHead( sizeof( PacketHeader ) );
    NetMessage msg;
    for( int msgIdx = 0; msgIdx < packet.m_header.m_message_count; ++msgIdx )
    {
        if( !packet.ExtractMessage( msg ) )
        {
            LOG_WARNING_TAG(
                "Net",
                "ExtractMessage Failed!" );
            return false;
        }

        if( !ProcessMessage( msg ) )
        {
            LOG_WARNING_TAG(
                "Net",
                "ProcessMessage Failed!" );
            return false;
        }
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

    if( def->RequiresConnection() && GetConnection( message.m_senderIdx ) == nullptr )
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

    if( def->IsReliable() )
    {
        NetConnection* connection = GetConnection( message.m_senderIdx );
        // message has already been processed before, ignore
        if( connection->HasReliableBeenProcessed( message.m_reliableID ) )
        {
            return true;
        }
        else
        {
            connection->MarkReliableProcessed( message.m_reliableID );
        }
    }

    if( def->IsInOrder() )
    {
        return ProcessInOrderMessage( message );
    }
    else
    {
        return RunMessageCallback( message );
    }
}

bool NetSession::RunMessageCallback( NetMessage& message )
{
    const NetMessageDefinition* def = message.m_def;
    MessageID msgID = message.GetMessageID();
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
        return false;
    }
    return true;
}

bool NetSession::ProcessInOrderMessage( NetMessage& message )
{
    const NetMessageDefinition* def = message.m_def;
    NetConnection* connection = GetConnection( message.m_senderIdx );
    NetMessageChannel* channel = connection->m_messageChannels[def->m_channelIdx];

    if( channel->IsMessageExpected( &message ) )
    {
        bool success;
        success = RunMessageCallback( message );
        channel->IncrementExpectedSequenceID();

        NetMessage* nextMessage = channel->PopMessageInOrder();
        while( nextMessage )
        {
            success = success & RunMessageCallback( *nextMessage );
            delete nextMessage;
            nextMessage = channel->PopMessageInOrder();
        }
        return success;
    }
    else
    {
        channel->CloneAndPushMessage( &message );
        return true;
    }
}

void NetSession::Flush( bool forced )
{
    if( m_state == eSessionState::DISCONNECTED )
        return;
    for( auto& pair : m_connections )
    {
        if( pair.second->IsClosed() )
            continue;

        if( forced )
            pair.second->Flush();
        else
            pair.second->FlushIfTimeUp();
    }
}

void NetSession::SendImmediate( const NetPacket& packet )
{
    m_packetChannel->SendImmediate( packet );
}

void NetSession::SendImmediateConnectionless( NetMessage* netMessage,
                                              const NetAddress& addr )
{
    NetPacket packet;
    packet.m_receiverAddress = addr;
    packet.SetWriteHead( sizeof( PacketHeader ) );
    PacketHeader& header = packet.m_header;
    header.m_senderConnectionIdx = INVALID_CONNECTION_INDEX;

    packet.WriteMessage( *netMessage );

    packet.PackHeader();
    SendImmediate( packet );
    delete netMessage;
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

bool NetSession::SendToAll( NetMessage* message )
{
    bool success = true;
    for( auto& pair : m_connections )
    {
        uint8 idx = pair.first;
        NetMessage* copyMsg = new NetMessage( *message );
        if( !SendToConnection( idx, copyMsg ) )
            success = false;
    }
    delete message;
    return success;
}

bool NetSession::SendToAllButMe( NetMessage* message )
{
    bool success = true;
    for( auto& pair : m_connections )
    {
        uint8 idx = pair.first;
        if( idx == m_myConnectionIdx )
            continue;
        NetMessage* copyMsg = new NetMessage( *message );
        if( !SendToConnection( idx, copyMsg ) )
            success = false;
    }
    delete message;
    return success;
}

bool NetSession::SendToHost( NetMessage* message )
{
    return SendToConnection( 0, message );
}

void NetSession::SetSimLoss( float lossAmount )
{
    m_simLossAmount = lossAmount;
}

void NetSession::SetSimLatency( uint minSimLatencyMS, uint maxSimLatencyMS /*= 0U */ )
{
    m_minSimLatency = (float) minSimLatencyMS / 1000;
    m_maxSimLatency = Max( m_minSimLatency, (float) maxSimLatencyMS / 1000 );
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


void NetSession::SetError( eSessionError error )
{
    m_lastError = error;
}

void NetSession::ClearError()
{
    m_lastError = eSessionError::OK;
}

eSessionError NetSession::GetLastError()
{
    eSessionError error = m_lastError;
    ClearError();
    return error;
}

bool NetSession::ReachedMaxClients()
{
    return m_connections.size() >= MAX_CONNECTION_COUNT;
}

uint8 NetSession::GetAvailableConnectionIdx()
{
    for( uint8 idx = 0; idx < MAX_CONNECTION_COUNT; ++idx )
    {
        if( GetConnection( idx ) != nullptr )
            continue;
        else
            return idx;
    }
    return INVALID_CONNECTION_INDEX;
}

void NetSession::CheckForTimeoutConnections()
{
    for( int idx = (int) m_unboundConnections.size() - 1; idx >= 0; --idx )
    {
        NetConnection* connection = m_unboundConnections[idx];
        if( connection->DidTimeout() )
        {
            if( connection == m_hostConnection )
            {
                m_hostConnection = nullptr;
                ShouldDisconnect();
            }
            if( connection == m_myConnection )
            {
                m_myConnection = nullptr;
                ShouldDisconnect();
            }
            delete connection;
            ContainerUtils::EraseAtIndexFast( m_unboundConnections, idx );
        }
    }

    for( auto iter = m_connections.cbegin(); iter != m_connections.cend(); )
    {
        NetConnection* connection = iter->second;
        if( connection->DidTimeout() )
        {
            if( connection == m_hostConnection )
            {
                m_hostConnection = nullptr;
                ShouldDisconnect();
            }
            if( connection == m_myConnection )
            {
                m_myConnection = nullptr;
                ShouldDisconnect();
            }
            delete connection;
            iter = m_connections.erase( iter ); // returned iter is next iter
        }
        else
        {
            ++iter;
        }
    }
}

void NetSession::UpdateNetClock()
{
    if( m_shouldResetClock )
    {
        if( IsHost() )
        {
            m_netClock->SetTime( Clock::GetRealTimeClock()->GetTimeSinceStartup() );
            m_shouldResetClock = false;
        }
        else if( m_lastReceivedHostTime != 0.f )
        {
            m_netClock->SetTime( m_lastReceivedHostTime );
            m_shouldResetClock = false;
        }
    }

    if( IsHost() )
    {
        m_netClock->Update( Clock::GetRealTimeClock()->GetDeltaSeconds() );
    }
    else
    {
        m_desiredClientTime = Lerp(
            m_lastReceivedHostTime, m_desiredClientTime,
            DESIRED_CLIENT_TIME_INERTIA );
        float internalDS = Clock::GetRealTimeClock()->GetDeltaSecondsF();
        m_desiredClientTime += internalDS;
        float timeDiff = m_desiredClientTime - m_netClock->GetTimeSinceStartupF();
        float appliedDS = Clampf(
            timeDiff,
            internalDS * ( 1 - MAX_NET_TIME_DILATION ),
            internalDS * ( 1 + MAX_NET_TIME_DILATION ) );

        m_netClock->Update( (double) appliedDS );
    }
}

bool NetSession::GreaterThanByTime::operator()(
    const NetPacket* lhs, const NetPacket* rhs ) const
{
    return lhs->m_timeOfReceive > rhs->m_timeOfReceive;
}
