#include "Engine/Net/NetMessage.hpp"


NetMessage::NetMessage( const string& name )
    : BytePacker( MESSAGE_MTU, m_localBuffer )
    , m_name( name )
{
    SetEndianness( Endianness::LITTLE );
    // Skip header
    SetWriteHead( 1 );
}


void NetMessage::WriteMessageHeader( uint8 msgIdx )
{
    SetWriteHead( 0 );
    Write( msgIdx );
}

uint8 NetMessage::ReadMessageIndex()
{
    uint8 msgIdx = INVALID_MESSAGE_INDEX;
    SetReadHead( 0 );
    Read( &msgIdx );
    return msgIdx;
}
