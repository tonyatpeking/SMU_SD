#include "Engine/Time/Timer.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"

Timer::Timer( Clock* clock, float lapTime )
    : m_clock( clock )
    , m_lapTime( lapTime )
    , m_timeOfStartLaps( clock->GetTimeSinceStartupF() )
{
    if( lapTime <= 0 )
    {
        LOG_WARNING( "lapTime cannot be 0 or less" );
        lapTime = 1;
    }
}

void Timer::Reset()
{
    m_timeOfStartLaps = m_clock->GetTimeSinceStartupF();
}

bool Timer::PeekOneLap()
{
    float currentTime = m_clock->GetTimeSinceStartupF();
    return ( currentTime - m_timeOfStartLaps ) > m_lapTime;
}

bool Timer::PopOneLap()
{
    if( PeekOneLap() )
    {
        m_timeOfStartLaps += m_lapTime;
        return true;
    }

    else
        return false;
}

int Timer::PopAllLaps()
{
    float currentTime = m_clock->GetTimeSinceStartupF();
    float passedTime =  currentTime - m_timeOfStartLaps;
    if( passedTime > m_lapTime )
    {
        int laps = RoundToInt( passedTime / m_lapTime );
        m_timeOfStartLaps += laps * m_lapTime;
        return laps;
    }

    return 0;

}
