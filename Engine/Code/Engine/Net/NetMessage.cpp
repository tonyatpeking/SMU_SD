#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetMessageDefinition.hpp"
#include "Engine/Net/NetMessageDatabase.hpp"
#include "Engine/Core/EngineCommonC.hpp"

NetMessage::NetMessage( const string& name )
    : BytePacker( MESSAGE_MTU, m_localBuffer )
    , m_name( name )
{
    SetEndianness( Endianness::LITTLE );
    m_def = NetMessageDatabase::GetDefinitionByName( name );
    ASSERT_OR_DIE( m_def != nullptr, "Could not find message definiton" );
    m_id = m_def->m_id;
    SetWriteHeadToPayload();
}

NetMessage::NetMessage()
    : BytePacker( MESSAGE_MTU, m_localBuffer )
{
    SetEndianness( Endianness::LITTLE );
}

MessageID NetMessage::GetMessageID()
{
    return m_id;
}

bool NetMessage::UnpackHeader()
{
    m_id = NET_MESSAGE_ID_INVALID;
    SetReadHead( 0 );
    if( !Read( &m_id ) )
        return false;

    m_def = NetMessageDatabase::GetDefinitionByID( m_id );
    if( m_def == nullptr )
    {
        LOG_WARNING_TAG(
            "Net",
            "No definition found for incomming message with ID %u",
            m_id );
        return false;
    }

    if( m_def->IsReliable() )
        if( !Read( &m_reliableID ) )
            return false;
    return true;
}

void NetMessage::PackHeader()
{
    size_t saveWriteHead = GetWrittenByteCount();
    SetWriteHead( 0 );
    Write( m_id );

    if( m_def->IsReliable() )
        Write( m_reliableID );

    SetWriteHead( saveWriteHead );
}

void NetMessage::SetWriteHeadToPayload()
{
    SetWriteHead( m_def->GetHeaderSize() );
}
