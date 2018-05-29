
// Time.cpp
//


#include "Engine/Time/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>



double InitializeTime( LARGE_INTEGER& out_initialTime )
{
    LARGE_INTEGER countsPerSecond;
    QueryPerformanceFrequency( &countsPerSecond );
    QueryPerformanceCounter( &out_initialTime );
    return( 1.0 / static_cast<double>( countsPerSecond.QuadPart ) );
}



double TimeUtils::GetCurrentTimeSeconds()
{
    static LARGE_INTEGER initialTime;
    static double secondsPerCount = InitializeTime( initialTime );
    LARGE_INTEGER currentCount;
    QueryPerformanceCounter( &currentCount );
    LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

    double currentSeconds = static_cast<double>( elapsedCountsSinceInitialTime ) * secondsPerCount;
    return currentSeconds;
}

TimeUtils::SysTime TimeUtils::GetDateTime()
{
    SYSTEMTIME lt;

    GetLocalTime( &lt );

    TimeUtils::SysTime sysTime;

    sysTime.year = lt.wYear;
    sysTime.month = lt.wMonth;
    sysTime.dayOfWeek = lt.wDayOfWeek;
    sysTime.day = lt.wDay;
    sysTime.hour = lt.wHour;
    sysTime.minute = lt.wMinute;
    sysTime.second = lt.wSecond;
    sysTime.milliseconds = lt.wMilliseconds;

    return sysTime;
}

