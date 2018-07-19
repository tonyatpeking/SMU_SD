#include "Engine/Core/Thread.hpp"

#include <thread>
#include <chrono>

namespace Thread
{


void Join( Handle handle )
{
    handle->join();
}

void Detach( Handle handle )
{
    handle->detach();
}

void SleepMS( int ms )
{
    std::this_thread::sleep_for( std::chrono::milliseconds(ms) );
}

void SleepS( float s )
{
    SleepMS( (int) ( s * 1000.f ) );
}

void ThreadYield()
{
    std::this_thread::yield();
}

}