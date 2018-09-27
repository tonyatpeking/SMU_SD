#pragma once
#include "Engine/Core/Types.hpp"
#include "Engine/Time/DateTime.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Log/LogLevel.hpp"


struct LogEntry
{
public:
    string m_tag;
    LogLevel m_level;
    string m_file;
    string m_function;
    int m_line = 0;
    DateTime m_dateTime;

    string m_text;
};
