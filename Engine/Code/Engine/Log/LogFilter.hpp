#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Time/DateTime.hpp"
#include "Engine/Log/LogEntry.hpp"

class LogFilter
{
public:

	LogFilter(){};

	~LogFilter(){};

    bool PassesFilter( const LogEntry& entry );

public:

    vector<string> m_tags;

    bool m_whitelistTags = false;  // flip this to whitelist/blacklist tags

    DateTime m_startDateTime;
    DateTime m_endDateTime;

};

