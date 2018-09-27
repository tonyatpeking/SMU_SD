#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Net/NetDefines.hpp"

#include <map>

struct NetAddress;
class NetPacket;
class BytePacker;
class NetConnection;
class UDPSocket;
class NetMessageDefinition;
class NetMessage;
class PacketChannel;



class NetSession
{
public:
    NetSession();
    ~NetSession();

    // message definitions
    bool RegisterMessageDefinition( const string& name, NetMessageCB cb );
    const NetMessageDefinition* GetMessageDefinition( const MessageID& id );
    const NetMessageDefinition* GetMessageDefinitionByIndex( const uint8_t idx ) const;
    void Finalize();

    // Starting a session (finalizes definitions - can't add more once
    // the session is running)
    void Bind( int port, uint rangeToTry = 0U );

    // Connection management
    NetConnection* AddConnection( uint idx, const NetAddress& addr );
    void CloseAllConnections();

    // updates
    void ProcessIncomming()
    {
        // receive packets, see who they came from
        // and call process those packets until
        // no more packets are available;
    }

    void ProcessOutgoing()
    {
        // foreach connection, process outgoing;
    }


public:
    vector<NetConnection*> m_connections; // all connections I know about;
    PacketChannel* m_channel; // what we send/receive packets on;

    // sorted, based on name
    vector<NetMessageDefinition*> m_messageDefinitions;
};