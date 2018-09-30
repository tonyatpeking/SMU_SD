#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetMessage.hpp"




NetPacket::NetPacket()
    : BytePacker( PACKET_MTU, m_localBuffer )
{
    SetEndianness( Endianness::LITTLE );
}

bool NetPacket::ReadHeader( PacketHeader& out_header )
{
    SetReadHead( 0 );
    size_t readByteCount;
    readByteCount = Read( &out_header.m_senderConnectionIdx );
    if( readByteCount != sizeof( uint8 ) )
        return false;
    readByteCount = Read( &out_header.m_unreliableCount );
    if( readByteCount != sizeof( uint8 ) )
        return false;
    return true;
}


void NetPacket::FinalizeHeader()
{
    m_headerFinalized = true;
    size_t savedWriteHead = GetWrittenByteCount();
    SetWriteHead( 0 );
    Write( m_header.m_senderConnectionIdx );
    Write( m_header.m_unreliableCount );
    SetWriteHead( savedWriteHead );
}

bool NetPacket::WriteUnreliableMessage( NetMessage const &msg )
{
    if( WriteMessage( msg ) );
    {
        ++m_header.m_unreliableCount;
        return true;
    }
    return false;
}

bool NetPacket::WriteMessage( NetMessage const &msg )
{
    m_headerFinalized = false;
    // [uint16 message_and_header_length]
    if( !Write( (uint16) msg.GetWrittenByteCount()) )
        return false;
    // [message index and payload]
    return WriteBytes( msg.GetWrittenByteCount(), msg.GetBuffer() );
}

bool NetPacket::ReadMessage( NetMessage& out_msg )
{
    uint16 messageLength;
    // Read [uint16 message_and_header_length]
    size_t readByteCount = Read( &messageLength );
    if( readByteCount != sizeof( uint16 ) )
        return false;
    // Read message
    out_msg.SetWriteHead( 0 );
    readByteCount = out_msg.CopyFrom( *this, messageLength );

    if( readByteCount != messageLength )
        return false;

    out_msg.m_sender = m_sender;
    out_msg.m_receiver = m_receiver;
    return true;
}
