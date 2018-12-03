#pragma once
#include "Engine/Net/NetCommonH.hpp"


class PacketTracker
{
public:
    void AddReliable( uint16 id );
    void Invalidate();
    float m_timeOfSend = 0;
    uint16 m_ack = INVALID_PACKET_ACK;

    uint16 m_sentReliableIds[MAX_RELIABLES_PER_PACKET];
    uint m_reliablesInPacket = 0;

};
