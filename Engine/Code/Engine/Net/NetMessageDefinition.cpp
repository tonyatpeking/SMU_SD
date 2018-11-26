#include "Engine/Net/NetMessageDefinition.hpp"
#include "Engine/Core/EngineCommonC.hpp"


NetMessageDefinition::NetMessageDefinition(
    const string& name,
    NetMessageCB cb,
    eNetMessageFlag flags,
    uint8 messageId,
    uint8 channelIdx )
    : m_name( name )
    , m_callback( cb )
    , m_flags( flags )
    , m_id( messageId )
    , m_channelIdx( channelIdx )
{
    CalculateHeaderSize();
}

void NetMessageDefinition::CalculateHeaderSize()
{
    m_headerSize = sizeof( MessageID );
    if( IsReliable() )
        m_headerSize += sizeof( ReliableID );
    if( IsInOrder() )
        m_headerSize += sizeof( SequenceID );
}

bool NetMessageDefinition::RequiresConnection() const
{
    return !AreBitsSet( (uint) m_flags, (uint) eNetMessageFlag::CONNECTIONLESS );
}

bool NetMessageDefinition::IsClientOnly() const
{
    return AreBitsSet( (uint) m_flags, (uint) eNetMessageFlag::CLIENT_ONLY );
}

bool NetMessageDefinition::IsHostOnly() const
{
    return AreBitsSet( (uint) m_flags, (uint) eNetMessageFlag::HOST_ONLY );
}

bool NetMessageDefinition::IsReliable() const
{
    return AreBitsSet( (uint) m_flags, (uint) eNetMessageFlag::RELIABLE );
}

bool NetMessageDefinition::IsInOrder() const
{
    return AreBitsSet( (uint) m_flags, (uint) eNetMessageFlag::IN_ORDER );
}
