#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Time/Time.hpp"

#include <deque>

namespace Profiler
{

#if defined( PROFILING_ENABLED )

//--------------------------------------------------------------------------------------
// internal

Measurement* CreateMeasurement( const char* id );
void DestroyMeasurementTreeR( Measurement* node );

Measurement* g_activeNode = nullptr;
// newer frames are in the front
std::deque<Measurement*> g_prevFrames;


bool g_isPaused = false;
bool g_shouldPauseNextFrame = false;


Measurement* CreateMeasurement( const char* id )
{
    Measurement* measure = new Measurement();

    // copy id, truncate if necessary
    for( int i = 0; i < MAX_STR_LEN - 1; ++i )
    {
        char c = id[i];
        measure->m_name[i] = c;
        if( c == '\0' )
            break;
    }
    measure->m_name[MAX_STR_LEN - 1] = '\0';

    // set time
    measure->m_startTime = TimeUtils::GetCurrentTimeSeconds();

    return measure;
}

void DestroyMeasurementTreeR( Measurement* node )
{
    // call recursively on all children
    // if no children end recursion
    Measurement* firstChild = node->m_firstChild;
    if( nullptr == firstChild )
    {
        delete node;
    }
    else
    {
        Measurement* currentChild = firstChild;
        Measurement* nextChild = nullptr;
        do
        {
            nextChild = currentChild->m_next;
            DestroyMeasurementTreeR( currentChild );
            // cannot call currentChild->m_next here because currentChild will be deleted
            currentChild = nextChild;
        } while( currentChild != firstChild );
    }
}


//--------------------------------------------------------------------------------------
// end internal

Scoped::Scoped( const char* tag )
{
    Push( tag );
}

Scoped::~Scoped()
{
    Pop();
}

void Measurement::AddChild( Measurement* newChild )
{
    if( m_firstChild )
    {
        newChild->m_next = m_firstChild->m_next;
        m_firstChild->m_next = newChild;
    }
    else
    {
        m_firstChild = newChild;
        newChild->m_next = newChild;
    }
}

void Measurement::SetParent( Measurement* parent )
{
    m_parent = parent;
}


double Measurement::GetElapsedTime()
{
    return m_endTime - m_startTime;
}

void Measurement::Finish()
{
    m_endTime = TimeUtils::GetCurrentTimeSeconds();
}

void StartUp()
{

}

void ShutDown()
{

}

void Push( const char* tag )
{
    if( g_isPaused )
        return;
    Measurement* measure = CreateMeasurement( tag );
    if( g_activeNode == nullptr )
    {
        g_activeNode = measure;
    }
    else
    {
        measure->SetParent( g_activeNode );
        g_activeNode->AddChild( measure );
        g_activeNode = measure;
    }
}

void Pop()
{
    if( g_isPaused )
        return;
    GUARANTEE_OR_DIE( g_activeNode != nullptr, "Someone called pop wihtout a push!" );
    g_activeNode->Finish();
    g_activeNode = g_activeNode->m_parent;
}

void MarkFrame()
{
    // check if there are pause or resume requests
    g_isPaused = g_shouldPauseNextFrame;

    if( g_isPaused )
        return;

    if( g_activeNode != nullptr )
    {
        if( g_prevFrames.size() >= MAX_HISTORY_LEN )
        {
            Measurement* last = g_prevFrames.back();
            g_prevFrames.pop_back();
            DestroyMeasurementTreeR( last );
        }

        g_prevFrames.push_front( g_activeNode );

        Pop();

        GUARANTEE_OR_DIE( g_activeNode == nullptr, "Someone forgot to pop!" );
    }

    Push( "Frame" );
}

Profiler::Measurement* GetPreviousFrame( int skipCount )
{
    if( skipCount >= g_prevFrames.size() )
        return nullptr;

    return g_prevFrames[skipCount];
}

void Pause()
{
    g_shouldPauseNextFrame = true;
}

void Resume()
{
    g_shouldPauseNextFrame = false;
}

#else

void Measurement::AddChild( Measurement* newChild ) { (void) ( newChild ); }

void Measurement::SetParent( Measurement* parent ) { (void) ( newChild ); }

void Measurement::Finish() {}

double Measurement::GetElapsedTime() { return 0; }

void StartUp() {}

void ShutDown() {}

void Push( const char* tag ) { (void) ( tag ); }

void Pop() {}

void MarkFrame() {}

Profiler::Measurement* GetPreviousFrame( int skipCount ) { (void) ( skipCount ); }

void Pause() {}

void Resume() {}

#endif



}