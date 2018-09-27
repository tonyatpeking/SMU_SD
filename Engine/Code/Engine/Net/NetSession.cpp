#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetPacket.hpp"
#include "Engine/DataUtils/BytePacker.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/UDPSocket.hpp"
#include "Engine/Net/NetCommand.hpp"


NetSession::NetSession()
{
    // Open up port for communication
}

NetSession::~NetSession()
{

}
//
//
// NetNode* NetSession::AddNode( UID uid, const NetAddress& address )
// {
//     // TODO: check for duplicates
//     NetNode* node = new NetNode();
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
// void NetSession::QueueSend( const string& netCommandName, BytePacker& data, NetNode receiver )
// {
//     // NetPacket* packet = GetOrCreateQueuedSendPacket( NetNode node );
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
