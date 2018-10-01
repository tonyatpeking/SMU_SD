#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetMessageDefinition.hpp"


void NetConnection::ProcessOutgoing()
{
    NetPacket packet;
    packet.m_receiver = this;
    while( !m_outboundUnreliables.empty() )
    {
        packet.SetWriteHead( sizeof(PacketHeader) );
        packet.m_header.m_senderConnectionIdx = m_owningSession->GetMyConnectionIdx();
        packet.m_header.m_unreliableCount = 0;
        FillPacketWithUnreliables( packet );
        packet.PackHeader();
        SendImmediate( packet );
    }
}

void NetConnection::QueueSend( NetMessage* netMsg )
{
    auto def = m_owningSession->GetMessageDefinitionByName( netMsg->m_name );
    if( def == nullptr )
    {
        LOG_ERROR_TAG( "Net", "Could not send message, no definition for %s",
                       netMsg->m_name.c_str() );
        return;
    }
    netMsg->WriteMessageHeader( def->m_id );
    m_outboundUnreliables.push( netMsg );
}

void NetConnection::SendImmediate( const NetPacket& packet )
{
    m_owningSession->SendImmediate( packet );
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
    while( !m_outboundUnreliables.empty() )
    {
        NetMessage* msg = m_outboundUnreliables.front();
        if( packet.WriteMessage( *msg ) )
        {
            delete msg;
            m_outboundUnreliables.pop();
            ++packet.m_header.m_unreliableCount;
        }
        // packet is full
        else
        {
            return;
        }
    }
}
