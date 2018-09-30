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
    const NetMessageDefinition* GetMessageDefinitionByName( const string& name );
    const NetMessageDefinition* GetMessageDefinitionByIndex( const MessageID idx ) const;


    // Starting a session (finalizes definitions - can't add more once
    // the session is running)
    void BindAndFinalize( int port, uint rangeToTry = 0U );

    // Connection management
    NetConnection* AddConnection( uint8 idx, const NetAddress& addr );
    NetConnection* GetConnection( uint8 idx );
    void CloseAllConnections();
    uint8 GetMyConnectionIdx() const { return INVALID_CONNECTION_INDEX; }

    // updates
    void ProcessIncomming();
    bool PacketIsValid( const NetPacket& packet );
    bool ProcessPacket( const NetPacket& packet );

    void ProcessOutgoing();

    void Send( const NetPacket& packet );

private:

    void SortMessageDefinitions();

    map<uint8, NetConnection*> m_connections; // all connections I know about;
    PacketChannel* m_packetChannel = nullptr; // what we send/receive packets on;

    // sorted, based on name
    vector<NetMessageDefinition*> m_messageDefinitions;
};