#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetMessageDatabase.hpp"



NetPacket::NetPacket()
    : BytePacker( PACKET_MTU, m_localBuffer )
{
    SetEndianness( Endianness::LITTLE );
}

void NetPacket::PackHeader()
{
    size_t savedWriteHead = GetWrittenByteCount();
    SetWriteHead( 0 );
    Write( m_header.m_senderConnectionIdx );
    Write( m_header.m_ack );
    Write( m_header.m_lastReceivedAck );
    Write( m_header.m_receivedAckBitfield );
    Write( m_header.m_unreliableCount );
    SetWriteHead( savedWriteHead );
}

bool NetPacket::UnpackHeader()
{
    SetReadHead( 0 );

    if( !Read( &m_header.m_senderConnectionIdx )
        || !Read( &m_header.m_ack )
        || !Read( &m_header.m_lastReceivedAck )
        || !Read( &m_header.m_receivedAckBitfield )
        || !Read( &m_header.m_unreliableCount ) )
        return false;

    return true;
}

bool NetPacket::WriteUnreliableMessage( NetMessage &msg )
{
    if( WriteMessage( msg ) )
    {
        ++m_header.m_unreliableCount;
        return true;
    }
    return false;
}

bool NetPacket::WriteMessage( NetMessage &msg )
{
    msg.PackHeader();
    // [uint16 message_and_header_length]
    if( !Write( (uint16) msg.GetWrittenByteCount() ) )
        return false;
    // [message index and payload]
    return WriteBytes( msg.GetWrittenByteCount(), msg.GetBuffer() );
}

bool NetPacket::ExtractMessage( NetMessage& out_msg )
{
    uint16 messageLength;
    // Read [uint16 message_and_header_length]
    if( !Read( &messageLength ) )
        return false;
    // Read message
    out_msg.SetWriteHead( 0 );
    uint16 readByteCount = (uint16) out_msg.CopyFrom( *this, messageLength );

    if( readByteCount != messageLength )
        return false;

    out_msg.m_senderAddress = m_senderAddress;
    out_msg.m_sender = m_sender;
    out_msg.m_receiver = m_receiver;

    if( !out_msg.UnpackHeader() )
        return false;

    return true;
}

bool NetPacket::IsValid()
{
    uint8 unreliableCounted = 0;

    SetReadHead( sizeof( PacketHeader ) );

    NetMessage msg;

    for( int msgIdx = 0; msgIdx < m_header.m_unreliableCount; ++msgIdx )
    {
        ++unreliableCounted;
        if( !ExtractMessage( msg ) )
            return false;
    }

    if( GetReadableByteCount() != 0
        || unreliableCounted != m_header.m_unreliableCount )
        return false;

    return true;
}
