#include "Engine/Core/ProfileLogScoped.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Time/Time.hpp"


ProfileLogScoped::ProfileLogScoped( const char* tag )
{
    m_tag = tag;
    m_timeStart = TimeUtils::GetCurrentTimeSeconds();
}

ProfileLogScoped::~ProfileLogScoped()
{
    double timeUsed = TimeUtils::GetCurrentTimeSeconds() - m_timeStart;
    Console::DefaultConsole()->Printf( "[PROFILER] [%s]:[%f ms]", m_tag, timeUsed * 1000 );
}
