#pragma once
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include <queue>

class NetMessage;
class NetSession;
class NetPacket;
class Timer;
class PacketTracker;

class NetConnection
{
public:

    NetConnection();
    ~NetConnection();

    void ProcessOutgoing();
    bool OnReceivePacket( NetPacket& packet, bool processSuccess );
    void QueueSend( NetMessage* netMsg );
    void SendImmediate( const NetPacket& packet );

    void SetClosed( bool closed ) { m_isClosed = closed; }

    void Close();

    bool IsClosed() const { return m_isClosed; }

    bool IsValid();

    // This will also pop and delete the unreliables in m_outboundUnreliables
    // Assumes that the write head is later than the PacketHeader
    // will update header with number of unreliables
    void FillPacketWithUnreliables( NetPacket& packet );

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
    void ConfirmPacketsReceivedByOther( uint16 ackISentBefore, uint16 ackBitfield );
    void ConfirmOnePacketReceivedByOther( uint16 ackISentBefore );

    // Packet Tracker
    PacketTracker* AddPacketTracker( uint16 ackIJustSent );
    PacketTracker* GetPackerTracker( uint16 ackISentBefore );

    // Loss Tracking
    void UpdateLossTracker( bool packetWasReceived );
    float CalculateLossRate();

public:

    NetAddress m_address;
    std::queue<NetMessage*> m_outboundUnreliables;
    bool m_isClosed = false;

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
    uint m_timeOfLastSendMS = 0;
    uint m_timeOfLastReceiveMS = 0;
    float m_lossRate = 0.f; //[0-1]
    uint m_roundTripTime = 0;
    float m_roundTripTimeInertia = 0.8f;  // [0,1]  higher values mean chages slower
};
