#pragma once
#include "Engine/Log/ProfileLogScoped.hpp"
#include "Game/EngineBuildPreferences.hpp"



#if defined( PROFILING_ENABLED )

#define PROFILER_PUSH(tag) Profiler::Push((#tag))
#define PROFILER_PUSH_FUNCTION() Profiler::Push(__FUNCTION__)
#define PROFILER_POP() Profiler::Pop()
#define PROFILER_SCOPED() Profiler::Scoped __profiler_##__LINE__ ## (__FUNCTION__)

#else

#define PROFILER_PUSH_TAG(tag) { (void)( #tag ); }
#define PROFILER_PUSH() ;
#define PROFILER_POP() ;
#define PROFILER_SCOPED() ;

#endif // PROFILING_ENABLED



namespace Profiler
{

class Scoped
{
public:
    Scoped( const char* tag );
    ~Scoped();
};

constexpr int MAX_STR_LEN = 64;
constexpr int MAX_HISTORY_LEN = 60 * 5;

struct Measurement
{
    ~Measurement();
    char m_name[MAX_STR_LEN];
    double m_startTime;
    double m_endTime;

    void AddChild( Measurement* newChild );
    void SetParent( Measurement* parent );
    double GetElapsedTime();
    void Finish();
    Measurement* m_parent = nullptr;
    Measurement* m_firstChild = nullptr;
    Measurement* m_next = nullptr;

};


void StartUp();
void ShutDown();

void Push( const char* tag );
void Pop();
void MarkFrame();

void SetVisible( bool visible );
void ToggleVisible();
void Render();
void ProcessInput();

Measurement* GetPreviousFrame( int skipCount = 0 ); // 0 is previous frame

void Pause();
void Resume();


};


