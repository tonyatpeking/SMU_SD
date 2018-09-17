#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include <map>

class NetPacket;
class NetCommand;
class BytePacker;
class NetNode;
class NetAddress;
class UDPSocket;

typedef int UID;
typedef uint16_t CommandID;

typedef void( *NetCommandCallback )( NetPacket& );
typedef size_t( *GetLengthCallback )( NetPacket& );

#define GAME_PORT "10084"
#define ETHERNET_MTU 1500  // maximum transmission unit - determined by hardware part of OSI model.
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it;
#define PACKET_MTU (ETHERNET_MTU - 40 - 8)

class NetSession
{
public:
    NetSession();
    ~NetSession();

    // Nodes
    NetNode* AddNode( UID uid, const NetAddress& address );

    // Communication
    void ProcessIncomming();
    void ProcessOutgoing();
    // data is copied out so it is save to delete after this is called
    // all commands are queued to allow for optimal packet size
    void QueueSend( const String& netCommandName, BytePacker& data, NetNode receiver );
    void SendImmediate( const NetPacket& packet );

    // NetCommands
    NetCommand* GetNetCommandFromID( CommandID id );
    // Note: read head for BytePacker must be at the command
    // this takes a packet to get the length because the command might need the context
    // of the packet to determine length
    size_t GetParameterLength( NetPacket& packet );
    // Note: read head for BytePacker must be at the command
    bool RunNetCommand( NetPacket& packet );
    bool RunNetCommands( NetPacket& packet );

    // walk through the packet and make sure all command and lengths are valid
    bool VerifyPacket( NetPacket& packet );

    void RegisterNetCommand( NetCommand* netCommand );



private:

    std::vector<NetPacket*> m_queuedSendPackets;

    std::map<CommandID, NetCommand*> m_netCommands;
    std::map<UID, NetNode*> m_nodes;

    UDPSocket* m_socket = nullptr;
};

