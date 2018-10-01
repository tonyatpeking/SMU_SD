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

bool NetSession::RegisterMessageDefinition( const string& name, NetMessageCB cb )
{
    for( auto& def : m_messageDefinitions )
    {
        if( def->m_name == name )
        {
            LOG_WARNING_TAG( "Net", "NetMessageDefinition already exists for [%s]",
                             name.c_str() );
            return false;
        }
    }

    NetMessageDefinition* def = new NetMessageDefinition( name, cb );
    m_messageDefinitions.push_back( def );
    return true;
}

const NetMessageDefinition* NetSession::GetMessageDefinitionByName( const string& name )
{
    for( auto& def : m_messageDefinitions )
    {
        if( name == def->m_name )
            return def;
    }
    return nullptr;
}

const NetMessageDefinition* NetSession::GetMessageDefinitionByID( const MessageID idx ) const
{
    if( m_messageDefinitions.size() <= idx )
        return nullptr;
    return m_messageDefinitions[idx];
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

    FinalizeMessageDefinitions();
}

NetConnection* NetSession::AddConnection( uint8 idx, const NetAddress& addr )
{
    if( ContainerUtils::ContainsKey( m_connections, idx ) )
    {
        LOG_WARNING_TAG( "Net", "Connection index [%d] already exists, replacing", idx );
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
    for ( auto& pair : m_connections )
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

NetAddress NetSession::GetMyAddress()
{
    return m_packetChannel->m_socket->m_address;
}

void NetSession::ProcessIncomming()
{
    NetPacket packet;
    while( m_packetChannel->Receive( packet ) )
    {
        if( !ProcessPacket( packet ) )
        {
            // bad packet, update connection with this info
            LOG_WARNING_TAG( "Net", "Bad Packet from %s",
                             packet.m_sender->m_address.ToStringAll().c_str() );
        }
    }
}

bool NetSession::ProcessPacket( NetPacket& packet )
{
    if( VALIDATE_PACKET && !packet.IsValid() )
        return false;
    packet.SetReadHead( sizeof( PacketHeader ) );
    NetMessage msg;
    for( int msgIdx = 0; msgIdx < packet.m_header.m_unreliableCount; ++msgIdx )
    {
        if( !packet.ReadMessage( msg ) )
            return false;
        if( !ProcessMessage( msg ) )
            return false;
    }
    return true;
}

bool NetSession::ProcessMessage( NetMessage& message )
{
    MessageID msgID = message.ReadMessageID();
    const NetMessageDefinition* def = GetMessageDefinitionByID( msgID );
    if( !def )
    {
        LOG_WARNING_TAG( "Net",
                         "No definition found for messageID [%d]",
                         msgID );
    }

    if( !def->m_callback( &message ) )
        return false;

    if( message.GetReadableByteCount() != 0 )
    {
        LOG_WARNING_TAG(
            "Net",
            "[%s] Message [%s] ran successfully but did not use all bytes",
            message.m_sender->m_address.ToStringAll().c_str(),
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

namespace
{

bool MessageNameCompare( const NetMessageDefinition* defA, const NetMessageDefinition* defB )
{
    return defA->m_name < defB->m_name;
}

}

void NetSession::FinalizeMessageDefinitions()
{
    std::sort( m_messageDefinitions.begin(), m_messageDefinitions.end(), MessageNameCompare );
    for( size_t messageID = 0; messageID < m_messageDefinitions.size(); ++messageID )
    {
        m_messageDefinitions[messageID]->m_id = (MessageID) messageID;
    }
}

//
//
// NetConnection* NetSession::AddNode( UID uid, const NetAddress& address )
// {
//     // TODO: check for duplicates
//     NetConnection* node = new NetConnection();
//     m_nodes[uid] = node;
//     return node;
// }
//
// void NetSession::ProcessIncomming()
// {
//     if( m_socket->IsClosed() )
//         return;
//
//     NetAddress fromAddr;
//
//     Byte buffer[PACKET_MTU];
//
//
//     while( true )
//     {
//         size_t read = m_socket->ReceiveFrom( fromAddr, buffer, PACKET_MTU );
//         if( read == 0 )
//         {
//             // someone closed connection
//             continue;
//         }
//         if( read == -1 )
//         {
//             // check if is fatal and do something
//             break;
//         }
//         // fill out a NetPacket with the buffer and address
//         NetPacket netPacket;
//
//         // run commands on the NetPacket
//         // bool success = RunNetCommands( netPacket);
//         // manage node health based on success
//         // drop a node if it is just sending crap
//     }
// }
//
// void NetSession::ProcessOutgoing()
// {
//     for ( auto& toSend : m_queuedSendPackets )
//     {
//         SendImmediate( *toSend );
//     }
// }
//
// void NetSession::QueueSend( const string& netCommandName, BytePacker& data, NetConnection receiver )
// {
//     // NetPacket* packet = GetOrCreateQueuedSendPacket( NetConnection node );
//     // packet->Append( netCommandName, data );
// }
//
// void NetSession::SendImmediate( const NetPacket& packet )
// {
//     m_socket->SendTo( packet.m_receiver.m_address,
//                       packet.m_bytePacker.GetBuffer(),
//                       packet.m_bytePacker.GetWrittenByteCount() );
// }
//
// NetCommand* NetSession::GetNetCommandFromID( CommandID id )
// {
//     // TODO: Not safe for map, will create empty entry if id does not exist
//     return m_netCommands[id];
// }
//
// size_t NetSession::GetParameterLength( NetPacket& packet )
// {
//     // get command id from packet
//     CommandID id = -1;
//     NetCommand* command = GetNetCommandFromID( id );
//     if( command->m_getLengthCallback == nullptr )
//         return command->m_fixedLength;
//     else
//         return command->m_getLengthCallback( packet );
// }
//
// bool NetSession::RunNetCommand( NetPacket& packet )
// {
//     // verify that after command has run, read head has moved exactly GetParameterLength
//     // or else raise error
//     return true;
// }
//
// bool NetSession::VerifyPacket( NetPacket& packet )
// {
//     if( packet.m_length != packet.m_bytePacker.GetWrittenByteCount() )
//         return false;
//     BytePacker& packer = packet.m_bytePacker;
//     size_t bytesCounted = 0;
//     while ( packer.GetReadHead() < packet.m_length )
//     {
//         CommandID id = -1;
//         // TODO implement Peek
//         //id = packer.Peek( &id );
//
//         if( GetNetCommandFromID( id ) == nullptr )
//             return false;
//
//         size_t length = GetParameterLength( packet );
//         bytesCounted += 2 + length;
//         packer.OffsetReadHead( 2 + length );
//     }
//     return bytesCounted == packet.m_length;
// }
//
// void NetSession::RegisterNetCommand( NetCommand* netCommand )
// {
//     //CommandID id = new unique id that is the same for all builds
//     //m_netCommands[id] = netCommand;
// }
//
// bool NetSession::RunNetCommands( NetPacket& packet )
// {
//     // while packet read head is not end
//     // RunNetCommand
//     return true;
// }
//
