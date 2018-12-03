#include "Engine/Log/ProfileLogScoped.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Core/ErrorUtils.hpp"

ProfileLogScoped::ProfileLogScoped( const char* tag )
{
    m_tag = tag;
    m_timeStart = TimeUtils::GetCurrentTimeSecondsD();
}

ProfileLogScoped::~ProfileLogScoped()
{
    double timeUsed = TimeUtils::GetCurrentTimeSecondsD() - m_timeStart;
    LOG_INFO_TAG( "Profiler", "[%s]:[%f ms]", m_tag, timeUsed * 1000 );
}
