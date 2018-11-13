#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Net/NetDefines.hpp"

#include <map>
#include <queue>

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
    NetConnection* GetMyConnection();
    NetAddress GetMyAddress();

    // updates
    void ProcessIncomming();

    bool ProcessPacket( NetPacket& packet );
    bool ProcessMessage( NetMessage& message );

    void ProcessOutgoing();

    void SendImmediate( const NetPacket& packet );
    bool SendToConnection( uint8 idx, NetMessage* message );

    // network condition simulation
    // 1 for all loss
    // 0 for no loss
    void SetSimLoss( float lossAmount );

    // if max is 0, range will be [min-min]
    void SetSimLatency( uint minAddedLatencyMS, uint maxAddedLatencyMS = 0U );

    bool ShouldDiscardPacketForLossSim() const;

    void SetSessionSendRate( float Hz );
    void SetConnectionSendRate( uint8 connectionIdx, float Hz );

    void SetHeartBeat( float Hz );

public:

    void ProcessIncommingWithLatency();
    void ProcessIncommingImmediate();

    map<uint8, NetConnection*> m_connections; // all connections I know about;
    uint8 m_myConnectionIdx = INVALID_CONNECTION_INDEX;
    PacketChannel* m_packetChannel = nullptr; // what we send/receive packets on;


    // Send rate
    float m_sendRate = DEFAULT_SESSION_SEND_RATE;

    // network condition simulation
    float m_simLossAmount = 0.f;
    uint m_minSimLatencyMS = 0U;
    uint m_maxSimLatencyMS = 0U;

    // using this for compare puts the oldest time at the top of the queue
    struct GreaterThanByTime
    {
        bool operator()( const NetPacket* lhs, const NetPacket* rhs ) const;
    };
    std::priority_queue<NetPacket*, vector<NetPacket*>, GreaterThanByTime> m_queuedPacketsToProcess;

};