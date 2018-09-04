#include "Engine/Log/LogFilter.hpp"



bool LogFilter::PassesFilter( const LogEntry& entry )
{
    if( m_whitelistTags )
    {
        for( auto& tag : m_tags )
        {
            if( tag == entry.m_tag ) // was in white list
                break;
        }
        return false; // was not in white list
    }
    else
    {
        for( auto& tag : m_tags )
        {
            if( tag == entry.m_tag ) // was in black list
                return false;
        }
    }

    DateTime emptyDateTime = DateTime{};
    if( m_startDateTime != emptyDateTime )
    {
        if( entry.m_dateTime < m_startDateTime )
            return false;
    }

    if( m_endDateTime != emptyDateTime )
    {
        if( entry.m_dateTime > m_endDateTime )
            return false;
    }

    return true;
}
