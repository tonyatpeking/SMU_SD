#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetPacket.hpp"


void NetConnection::ProcessOutgoing()
{
    NetPacket packet;
    while( !m_outboundUnreliables.empty() )
    {
        PacketHeader header;
        header.m_senderConnectionIdx = m_owningSession->GetMyConnectionIdx();
        header.m_unreliableCount = 0;

        packet.WriteHeader( header );
        FillPacketWithUnreliables( packet );
        SendImmediate( packet );
    }
}

void NetConnection::QueueSend( NetMessage* netMsg )
{
    m_outboundUnreliables.push( netMsg );
}

void NetConnection::SendImmediate( const NetPacket& packet )
{
    m_owningSession->Send( packet );
}

void NetConnection::Close()
{
    SetClosed( true );
}

bool NetConnection::IsValid()
{
    return !IsClosed() && !m_address.IsInvalid();
}

void NetConnection::FillPacketWithUnreliables( NetPacket& packet )
{
    PacketHeader packetHeader;
    uint8 unreliableMsgCount = 0;
    packetHeader.m_senderConnectionIdx = m_owningSession->GetMyConnectionIdx();

    PacketHeader



    packet.WriteMessage()
}
