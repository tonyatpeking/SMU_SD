#include "Engine/Core/EngineCommonC.hpp"


//calls a function N times, to test performance and memory leaks
void CallFunctinoNTimes( std::function<void()> func, int timesToCall )
{
    for( int i = 0; i < timesToCall; i++ )
    {
        func();
    }
}

