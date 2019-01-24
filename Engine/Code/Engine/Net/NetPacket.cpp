#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetMessageDatabase.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Net/NetMessageDefinition.hpp"

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
    Write( m_header.m_message_count );
    SetWriteHead( savedWriteHead );
}

bool NetPacket::UnpackHeader()
{
    SetReadHead( 0 );

    if( !Read( &m_header.m_senderConnectionIdx )
        || !Read( &m_header.m_ack )
        || !Read( &m_header.m_lastReceivedAck )
        || !Read( &m_header.m_receivedAckBitfield )
        || !Read( &m_header.m_message_count ) )
        return false;

    return true;
}


bool NetPacket::WriteMessage( NetMessage &msg )
{
    msg.PackHeader();
    if( !CanWriteMessage( msg ) )
        return false;
    // [uint16 message_and_header_length]
    Write( ((uint16) msg.GetWrittenByteCount()) );
    bool success = WriteBytes( msg.GetWrittenByteCount(), msg.GetBuffer() );
    if( !success )
        int i = 1;
    ++m_header.m_message_count;

    if( msg.m_def->IsReliable() )
        ++m_reliableMessageCount;

    return true;
}

bool NetPacket::CanWriteMessage( NetMessage& msg )
{
    if( m_header.m_message_count == MAX_MESSAGES_PER_PACKET )
        return false;

    if( msg.m_def->IsReliable()
        && m_reliableMessageCount == MAX_RELIABLES_PER_PACKET )
        return false;

    // the uint16 is the size of the message
    size_t spaceNeeded = msg.GetWrittenByteCount() + sizeof( uint16 );
    if( m_bufferSize - m_writeHead < spaceNeeded )
        return false;
    return true;
}

bool NetPacket::ExtractMessage( NetMessage& out_msg )
{
    uint16 messageLength;
    // Read [uint16 message_and_header_length]
    if( !Read( &messageLength ) )
    {
        LOG_WARNING_TAG( "Net", "Could not read message length" );
        return false;
    }
    // Read message
    out_msg.SetWriteHead( 0 );
    uint16 readByteCount = (uint16) out_msg.CopyFrom( *this, messageLength );

    if( readByteCount != messageLength )
    {
        LOG_WARNING_TAG( "Net",
                         "readByteCount != messageLength %d, %d",
                         readByteCount,
                         messageLength);
        return false;
    }

    out_msg.m_senderAddress = m_senderAddress;
    out_msg.m_senderIdx = m_senderIdx;
    out_msg.m_receiverIdx = m_receiverIdx;

    if( !out_msg.UnpackHeader() )
    {
        LOG_WARNING_TAG( "Net", "message UnpackHeader failed" );
        return false;
    }

    return true;
}

bool NetPacket::IsValid()
{
    uint8 msgCounted = 0;

    SetReadHead( sizeof( PacketHeader ) );

    NetMessage msg;

    for( int msgIdx = 0; msgIdx < m_header.m_message_count; ++msgIdx )
    {
        ++msgCounted;
        if( !ExtractMessage( msg ) )
        {
            LOG_WARNING_TAG( "Net", "ExtractMessage Failed!" );
            return false;
        }
    }

    if( GetReadableByteCount() != 0 )
    {
        LOG_WARNING_TAG( "Net", "Read all bytes Check Failed!" );
        return false;
    }

    if( msgCounted != m_header.m_message_count )
    {
        LOG_WARNING_TAG( "Net", "Message Count match header Failed!" );
        return false;
    }

    return true;
}
