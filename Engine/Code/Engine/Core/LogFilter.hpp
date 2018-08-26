#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Time/DateTime.hpp"
#include "Engine/Core/LogEntry.hpp"

class LogFilter
{
public:

	LogFilter(){};

	~LogFilter(){};

    bool PassesFilter( const LogEntry& entry );

public:

    std::vector<String> m_tags;

    bool m_whitelistTags = false;  // flip this to whitelist/blacklist tags

    DateTime m_startDateTime;
    DateTime m_endDateTime;

};

