#include "Engine/Net/NetMessage.hpp"


NetMessage::NetMessage( const string& name )
    : BytePacker( MESSAGE_MTU, m_localBuffer )
    , m_name( name )
{
    SetEndianness( Endianness::LITTLE );
    // Skip header
    SetWriteHead( 1 );
}


NetMessage::NetMessage()
    : BytePacker( MESSAGE_MTU, m_localBuffer )
{
    SetEndianness( Endianness::LITTLE );
    // Skip header
    SetWriteHead( 1 );
}

void NetMessage::WriteMessageHeader( MessageID msgID )
{
    size_t saveWriteHead = GetWrittenByteCount();
    SetWriteHead( 0 );
    Write( msgID );
    SetWriteHead( saveWriteHead );
}

MessageID NetMessage::ReadMessageID()
{
    MessageID msgID = INVALID_MESSAGE_ID;
    SetReadHead( 0 );
    Read( &msgID );
    return msgID;
}
