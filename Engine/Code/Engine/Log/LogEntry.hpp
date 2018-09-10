#pragma once
#include "Engine/Core/Types.hpp"
#include "Engine/Time/DateTime.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Log/LogLevel.hpp"


struct LogEntry
{
public:
    String m_tag;
    LogLevel m_level;
    String m_file;
    String m_function;
    int m_line = 0;
    DateTime m_dateTime;

    String m_text;
};
