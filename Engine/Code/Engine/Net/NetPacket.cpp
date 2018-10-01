#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetMessage.hpp"




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
    Write( m_header.m_unreliableCount );
    SetWriteHead( savedWriteHead );
}

bool NetPacket::UnpackHeader()
{
    SetReadHead( 0 );

    if( !Read( &m_header.m_senderConnectionIdx )
        || !Read( &m_header.m_unreliableCount ) )
        return false;

    return true;
}

bool NetPacket::WriteUnreliableMessage( NetMessage const &msg )
{
    if( WriteMessage( msg ) )
    {
        ++m_header.m_unreliableCount;
        return true;
    }
    return false;
}

bool NetPacket::WriteMessage( NetMessage const &msg )
{
    // [uint16 message_and_header_length]
    if( !Write( (uint16) msg.GetWrittenByteCount() ) )
        return false;
    // [message index and payload]
    return WriteBytes( msg.GetWrittenByteCount(), msg.GetBuffer() );
}

bool NetPacket::ReadMessage( NetMessage& out_msg )
{
    uint16 messageLength;
    // Read [uint16 message_and_header_length]
    if( !Read( &messageLength ) )
        return false;
    // Read message
    out_msg.SetWriteHead( 0 );
    uint16 readByteCount = out_msg.CopyFrom( *this, messageLength );

    if( readByteCount != messageLength )
        return false;

    out_msg.m_sender = m_sender;
    out_msg.m_receiver = m_receiver;
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
        if( !ReadMessage( msg ) )
            return false;
    }

    if( GetReadableByteCount() != 0
        || unreliableCounted != m_header.m_unreliableCount )
        return false;

    return true;
}
