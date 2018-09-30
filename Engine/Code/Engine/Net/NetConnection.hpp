#pragma once
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include <queue>

class NetMessage;
class NetSession;
class NetPacket;

class NetConnection
{
public:

    NetConnection() {};
    ~NetConnection() {};

    void ProcessOutgoing();
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

public:
    NetAddress m_address;

    std::queue<NetMessage*> m_outboundUnreliables;

    NetSession* m_owningSession = nullptr;

    uint8 m_idxInSession = (uint8) -1;

    bool m_isClosed = false;
};
