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
    friend class NetSessionDisplay;
public:
    static NetSession* GetDefault();
    NetSession();
    ~NetSession();

    // message definitions
    bool RegisterMessageDefinition( const string& name, NetMessageCB cb );
    const NetMessageDefinition* GetMessageDefinitionByName( const string& name );
    const NetMessageDefinition* GetMessageDefinitionByID( const MessageID idx ) const;


    // Starting a session (finalizes definitions - can't add more once
    // the session is running)
    void BindAndFinalize( int port, uint rangeToTry = 0U );

    // Connection management
    NetConnection* AddConnection( uint8 idx, const NetAddress& addr );
    NetConnection* AddConnectionSelf( uint8 idx );
    NetConnection* GetConnection( uint8 idx );
    NetConnection* GetConnection( const NetAddress& addr );
    void CloseAllConnections();
    uint8 GetMyConnectionIdx() const { return m_myConnectionIdx; }
    NetAddress GetMyAddress();

    // updates
    void ProcessIncomming();
    bool ProcessPacket( NetPacket& packet );
    bool ProcessMessage( NetMessage& message );

    void ProcessOutgoing();

    void SendImmediate( const NetPacket& packet );
    bool SendToConnection( uint8 idx, NetMessage* message );

private:

    void FinalizeMessageDefinitions();

    map<uint8, NetConnection*> m_connections; // all connections I know about;
    uint8 m_myConnectionIdx = INVALID_CONNECTION_INDEX;
    PacketChannel* m_packetChannel = nullptr; // what we send/receive packets on;

    // sorted, based on name
    vector<NetMessageDefinition*> m_messageDefinitions;
};