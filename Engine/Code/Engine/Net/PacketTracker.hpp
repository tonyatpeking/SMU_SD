#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Net/NetDefines.hpp"


class PacketTracker
{
public:
    uint m_timeOfSendMS = 0;
    uint16 m_ack = INVALID_PACKET_ACK;


};
