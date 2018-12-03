
// Time.cpp
//


#include "Engine/Time/Time.hpp"

#include "Engine/Core/WindowsCommon.hpp"

#include <ctime>



double InitializeTime( LARGE_INTEGER& out_initialTime )
{
    LARGE_INTEGER countsPerSecond;
    QueryPerformanceFrequency( &countsPerSecond );
    QueryPerformanceCounter( &out_initialTime );
    return( 1.0 / static_cast<double>( countsPerSecond.QuadPart ) );
}



double TimeUtils::GetCurrentTimeSecondsD()
{
    static LARGE_INTEGER initialTime;
    static double secondsPerCount = InitializeTime( initialTime );
    LARGE_INTEGER currentCount;
    QueryPerformanceCounter( &currentCount );
    LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

    double currentSeconds = static_cast<double>( elapsedCountsSinceInitialTime ) * secondsPerCount;
    return currentSeconds;
}

float TimeUtils::GetCurrentTimeSecondsF()
{
    return (float)GetCurrentTimeSecondsD();
}

uint TimeUtils::GetCurrentTimeMS()
{
    return (uint) ( GetCurrentTimeSecondsD() * 1000 );
}

