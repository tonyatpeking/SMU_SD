#pragma once

#include "Engine/Net/NetCommonH.hpp"
#include "Engine/Net/NetAddress.hpp"

#include <map>
#include <queue>

class Clock;

class NetSession
{
    friend class NetSessionDisplay;
public:
    static NetSession* GetDefault();
    NetSession();
    ~NetSession();


    void Host( const string& myID, int port, uint rangeToTry = 0U );
    void Join( const string& myID, const NetAddress& hostAddr );
    void Disconnect();
    void ShouldDisconnect();
    bool IsHost();
    void SendJoinRequestWithInterval();
    void ResetJoinTimeout();
    bool DidJoinTimeout();

    // Hangup, also flushes
    void SendHangupToAll();

    // Message processing
    bool ProcessJoinRequest( NetMessage* msg );
    bool ProcessJoinDeny( NetMessage* msg );
    bool ProcessJoinAccept( uint8 assignedIdx );
    bool ProcessNewConnection( NetMessage* msg );
    bool ProcessJoinFinished( NetMessage* msg );
    bool ProcessUpdateConnectionState( NetMessage* msg, eConnectionState state );
    bool ProcessHangup( NetMessage* msg );
    bool ProcessHeartbeat( NetMessage* msg, uint hostTimeMS );

    // Starting a session (finalizes definitions - can't add more once
    // the session is running)
    bool BindToPort( int port, uint rangeToTry = 0U );
    void Finalize();

    // Connection management
    NetConnection* AddConnection( uint8 idx, const NetAddress& addr );
    NetConnection* AddConnectionSelf( uint8 idx );
    NetConnection* GetConnection( uint8 idx );
    NetConnection* GetConnection( const NetAddress& addr );
    void CloseAllConnections();
    uint8 GetMyConnectionIdx() const { return m_myConnectionIdx; }
    NetConnection* GetMyConnection();
    NetAddress GetMyAddress();

    NetConnection* CreateConnection( const NetAddress& addr );
    void DestroyConnection( NetConnection* connection );
    void BindConnection( uint8 idx, NetConnection* connection );

    // updates
    void Update();

    bool ProcessPacket( NetPacket& packet );
    bool ProcessMessage( NetMessage& message );
    bool RunMessageCallback( NetMessage& message );
    // returns true if message was queued or if ran successfully
    // returns false if any messages in queue failed
    bool ProcessInOrderMessage( NetMessage& message );

    void Flush(bool forced = false);

    void SendImmediate( const NetPacket& packet );
    // takes ownership of netMessage, only works for connectionless
    void SendImmediateConnectionless( NetMessage* netMessage,
                                      const NetAddress& addr );
    bool SendToConnection( uint8 idx, NetMessage* message ); // takes ownership of netMessage

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

    // errors
    void SetError( eSessionError error );
    void ClearError();
    eSessionError GetLastError();

    bool ReachedMaxClients();
    uint8 GetAvailableConnectionIdx();

    void CheckForTimeoutConnections();

    // Net Clock
    void UpdateNetClock();
    Clock* GetNetClock() { return m_netClock; };

public:

    void ProcessIncommingWithLatency();
    void ProcessIncommingImmediate();

    // Connections
    vector<NetConnection*> m_unboundConnections;
    map<uint8, NetConnection*> m_connections; // all bound connections I know about;
    NetConnection* m_myConnection = nullptr;
    NetConnection* m_hostConnection = nullptr;
    NetAddress m_boundAddress;
    Timer* m_joinTimeoutTimer = nullptr;

    uint8 m_myConnectionIdx = INVALID_CONNECTION_INDEX;
    PacketChannel* m_packetChannel = nullptr; // what we send/receive packets on;


    // Send rate
    float m_sendRate = DEFAULT_SESSION_SEND_RATE;

    // network condition simulation
    float m_simLossAmount = 0.f;
    float m_minSimLatency = 0.f;
    float m_maxSimLatency = 0.f;

    // using this for compare puts the oldest time at the top of the queue
    struct GreaterThanByTime
    {
        bool operator()( const NetPacket* lhs, const NetPacket* rhs ) const;
    };
    std::priority_queue<NetPacket*, vector<NetPacket*>, GreaterThanByTime> m_queuedPacketsToProcess;

    // error
    eSessionError m_lastError = eSessionError::OK;

    // session state
    eSessionState m_state = eSessionState::DISCONNECTED;

    // net clock
    float m_lastReceivedHostTime = 0.f;
    float m_desiredClientTime = 0.f;
    Clock* m_netClock = nullptr;
    bool m_shouldResetClock = false;
};