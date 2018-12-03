#pragma once
#include "Engine/Net/NetCommonH.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/NetConnectionInfo.hpp"
#include <queue>

class Timer;

class NetConnection
{
public:

    NetConnection();
    ~NetConnection();

    void FlushIfTimeUp();
    void Flush();
    bool OnReceivePacket( NetPacket& packet, bool processSuccess );
    void QueueSend( NetMessage* netMsg );
    void SendImmediate( const NetPacket& packet );

    void SetClosed( bool closed ) { m_isClosed = closed; }

    void Close();

    bool IsClosed() const { return m_isClosed; }

    bool IsValid();

    // This will also pop and delete the unreliables in m_unsentUnreliables
    // Assumes that the write head is later than the PacketHeader
    // will update header with number of unreliables
    void FillPacketWithUnsentUnreliables( NetPacket& packet );
    void FillPacketWithUnconfirmedReliables( NetPacket& packet );
    void FillPacketWithUnsentReliables( NetPacket& packet );
    void ClearUnreliables();

    void SetSendRate( float hz );
    float GetEffectiveSendInterval() const;

    // checks if lap time has changed
    // also resets clock if lapped
    bool PopSendTimer();

    // checks if heartbeat rate has changed
    // also resets clock if lapped
    bool PopHeartbeatTimer();


    // Acks
    void UpdateLastReceivedAck( uint16 ackFromOther );
    void ConfirmPacketsReceivedByOther(
        uint16 ackISentBefore, uint16 ackBitfield );
    void ConfirmOnePacketReceivedByOther( uint16 ackISentBefore );

    // Packet Tracker
    PacketTracker* GetCurrentPacketTracker();
    PacketTracker* AddPacketTracker( uint16 ackIJustSent );
    PacketTracker* GetPackerTracker( uint16 ackISentBefore );

    // Loss Tracking
    void UpdateLossTracker( bool packetWasReceived );
    float CalculateLossRate();

    // Reliable
    float GetReliableResendWait();
    void ConfirmReliable( uint16 reliableID );
    bool HasReliableBeenProcessed( uint16 reliableID );
    void MarkReliableProcessed( uint16 reliableID );
    uint16 GetOldestUnconfirmedReliableID(); // cyclic lowest
    bool CanSendNewReliable();

    // Connection
    bool IsMe() const;
    bool IsHost() const;
    bool IsClient() const;
    bool IsConnected() const;
    bool IsDisconnected() const;
    bool IsReady() const;

    // state
    void SetState( eConnectionState state );

    // timeout
    bool DidTimeout();

public:

    NetAddress m_address;
    bool m_isClosed = false;


    // messages
    std::queue<NetMessage*> m_unsentUnreliables;

    // reliables
    std::queue<NetMessage*> m_unsentReliables;
    std::vector<NetMessage*> m_unconfirmedReliables;
    uint16 m_nextReliableID = 65530;
    std::vector<uint16> m_receivedReliableIDs;
    uint16 m_highestReceivedReliabeID = 65530;

    // in order traffic
    NetMessageChannel* m_messageChannels[MAX_MESSAGE_CHANNELS];

    // Session
    NetSession* m_owningSession = nullptr;
    uint8 m_idxInSession = (uint8) -1;

    // Net tick
    float m_sendRate = MAX_SEND_RATE;
    Timer* m_sendTimer = nullptr;

    // Heartbeat
    float m_heartbeatRate = DEFAULT_HEARTBEAT_RATE;
    Timer* m_heartbeatTimer = nullptr;

    // Acks
    uint16 m_nextAckToSend = 0U; // my ack counter
    uint16 m_lastReceivedAck = INVALID_PACKET_ACK; // 4    -1
    uint16 m_receivedAcksCount = 0;
    uint16 m_receivedAckBitfield = 0;  // 1 2 3          0 0 0
    bool m_shouldForceSend = false;

    // Packet tracking
    vector<PacketTracker*> m_packetTrackers;
    size_t m_nextFreePacketTrackerSlot = 0;
    vector<uint8> m_packetLossTracker; // 1 for packed received, 0 for lost
    size_t m_nextFreeLossTrackerSlot = 0;

    // Analytics
    float m_timeOfLastSend = 0.f;
    float m_timeOfLastReceive = 0.f;
    float m_lossRate = 0.f; //[0-1]
    float m_roundTripTime = 0;
    float m_roundTripTimeInertia = ROUND_TRIP_TIME_INERTIA;

    // Connection
    NetConnectionInfo m_info;
    eConnectionState m_state = eConnectionState::DISCONNECTED;
};
