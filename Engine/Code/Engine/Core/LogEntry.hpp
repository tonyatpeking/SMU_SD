#pragma once
#include "Engine/Core/Types.hpp"
#include "Engine/Time/DateTime.hpp"

struct LogEntry
{
public:
    String m_tag;
    String m_text;
    DateTime m_dateTime;
};
