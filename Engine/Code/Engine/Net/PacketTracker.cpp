#include "Engine/Net/PacketTracker.hpp"



void PacketTracker::AddReliable( uint16 id )
{
    m_sentReliableIds[m_reliablesInPacket] = id;
    ++m_reliablesInPacket;
}

void PacketTracker::Invalidate()
{
    m_ack = INVALID_PACKET_ACK;
    m_timeOfSend = 0.f;
    m_reliablesInPacket = 0;
}
