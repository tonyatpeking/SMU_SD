#include "Engine/Net/NetMessageDatabase.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Net/NetMessageDefinition.hpp"
#include <algorithm>


namespace
{

bool MessageNameCompare( const NetMessageDefinition* defA,
                         const NetMessageDefinition* defB )
{
    return defA->m_name < defB->m_name;
}
}

bool NetMessageDatabase::RegisterDefinition(
    const string& name,
    NetMessageCB cb,
    eNetMessageFlag flags /*= eNetMessageFlag::DEFAULT*/,
    uint8 messageId /*= NET_MESSAGE_ID_AUTO*/,
    uint8 channelIdx /*= 0 */ )
{
    vector<NetMessageDefinition*>& defs = GetMessageDefinitions();

    for( auto& def : defs )
    {
        if( def->m_name == name )
        {
            LOG_WARNING_TAG( "Net", "NetMessageDefinition already exists for [%s]",
                             name.c_str() );
            return false;
        }
        if( def->m_id == messageId && def->m_id != NET_MESSAGE_ID_AUTO )
        {
            LOG_WARNING_TAG( "Net", "NetMessageDefinition id already exists for [%u]",
                             messageId );
            return false;
        }
    }

    NetMessageDefinition* def = new NetMessageDefinition(
        name, cb, flags, messageId, channelIdx );

    defs.push_back( def );
    return true;
}

bool NetMessageDatabase::Finalize()
{
    vector<NetMessageDefinition*>& defs = GetMessageDefinitions();

    std::sort(
        defs.begin(),
        defs.end(),
        MessageNameCompare );

    constexpr size_t MAX_MESSAGE_COUNT = 1 << sizeof( MessageID ) * 8;
    NetMessageDefinition* tempDefs[MAX_MESSAGE_COUNT];

    for( int idx = 0; idx < MAX_MESSAGE_COUNT; ++idx )
        tempDefs[idx] = nullptr;

    // loop through all and assign all with fixed index to tempDefs
    for( size_t idx = 0; idx < defs.size(); ++idx )
    {
        NetMessageDefinition* def = defs[idx];
        if( def->m_id != NET_MESSAGE_ID_AUTO )
            tempDefs[def->m_id] = def;
    }

    // loop through all again and assign all with auto index to tempDefs
    size_t currentIdx = 0;
    for( size_t idx = 0; idx < defs.size(); ++idx )
    {
        NetMessageDefinition* def = defs[idx];
        if( def->m_id == NET_MESSAGE_ID_AUTO )
        {
            while( tempDefs[currentIdx] != nullptr )
                ++currentIdx;
            tempDefs[currentIdx] = def;
            def->m_id = (MessageID) currentIdx;
        }
    }

    // assign back to vector
    size_t capacity = defs.size();
    defs.clear();
    defs.reserve( capacity );
    for( int idx = 0; idx < MAX_MESSAGE_COUNT; ++idx )
    {
        if( tempDefs[idx] != nullptr )
        {
            defs.push_back( tempDefs[idx] );
        }
    }

    // print out all messages and idx
    LOG_INFO_TAG( "Net", "The following net messages have been resgistered:" );
    for ( auto& def : defs )
    {
        LOG_INFO_TAG( "Net", "  %s, %u", def->m_name.c_str(), def->m_id );
    }

    return true;
}

const NetMessageDefinition* NetMessageDatabase::GetDefinitionByName(
    const string& name )
{
    vector<NetMessageDefinition*>& defs = GetMessageDefinitions();
    for( auto& def : defs )
    {
        if( name == def->m_name )
            return def;
    }
    return nullptr;
}

const NetMessageDefinition* NetMessageDatabase::GetDefinitionByID(
    const MessageID idx )
{
    vector<NetMessageDefinition*>& defs = GetMessageDefinitions();
    for( auto& def : defs )
    {
        if( idx == def->m_id )
            return def;
    }
    return nullptr;
}

vector<NetMessageDefinition*>& NetMessageDatabase::GetMessageDefinitions()
{
    static vector<NetMessageDefinition*> s_messageDefinitions;
    return s_messageDefinitions;
}
