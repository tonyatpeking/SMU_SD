#pragma once
#include <thread>
#include "Engine/Core/EngineCommonH.hpp"

namespace Thread
{
typedef std::thread* Handle;

template <typename FuntionT, typename... Args>
Handle Create( FuntionT&& cb, Args&&... args )
{
    return new std::thread( cb, args... );
}

// Join will wait until the thread is complete before return control to the calling thread
void Join( Handle handle );

// Detach will return control immediately, and the thread will be freed automatically when complete;
void Detach( Handle handle );

template <typename FuntionT, typename... Args>
void CreateAndDetach( FuntionT&& cb, Args&&... args )
{
    Handle handle = Create( cb, args... );
    handle->detach();
}

// Control
void SleepMS( int ms );
void SleepS( float s );
void ThreadYield();



// Debug
// void ThreadSetName( char const *name );


// Future reference

// CoreAffinity -> Which Processor/Core do we prefer to run this thread on;
// void ThreadSetCoreAffinity( uint core_bitfield );
// Priority -> How important is it that this thread run (lower means it may be
// get starved out.  Higher means it could starve others).
// void ThreadSetPriority( uint priority );

}