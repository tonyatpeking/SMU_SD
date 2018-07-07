#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/SmartEnum.hpp"
#include "Engine/Core/ProfilerReport.hpp"
#include "Engine/Core/ProfilerReportEntry.hpp"

#include <deque>

namespace Profiler
{

#if defined( PROFILING_ENABLED )

//--------------------------------------------------------------------------------------
// internal

SMART_ENUM(
    DisplayHierarchy,

    FLAT,
    TREE
)

SMART_ENUM(
    DisplaySortMode,

    SELF,
    TOTAL
)

Measurement* CreateMeasurement( const char* id );
void DestroyMeasurementTreeR( Measurement* node );

Measurement* g_activeNode = nullptr;
// newer frames are in the front
std::deque<Measurement*> g_prevFrames;

bool g_isPaused = false;
bool g_shouldPauseNextFrame = false;

bool g_visible = false;
bool g_cursorSettingsPushed = false;

float g_updateIntervalSeconds = 0.1f;

DisplayHierarchy g_hierarchy = DisplayHierarchy::TREE;
DisplaySortMode g_sortMode = DisplaySortMode::TOTAL;



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

void SetVisible( bool visible )
{
    g_visible = visible;
}

void ToggleVisible()
{
    g_visible = !g_visible;
}

void RenderReport( int frameSkip = 0 )
{
    Renderer* renderer = Renderer::GetDefault();
    Window* window = renderer->GetWindow();
    AABB2 bounds = window->GetWindowBounds();
    float fontHeight = Console::DefaultConsole()->GetFontHeight();

    static float timeOfLastUpdate = 0.f;
    static double frameTime = 0;
    static double fps = 0;
    bool shouldUpdateThisFrame = false;

    float currentTime = (float)TimeUtils::GetCurrentTimeSeconds();
    if( currentTime - timeOfLastUpdate > g_updateIntervalSeconds )
    {
        shouldUpdateThisFrame = true;
    }

    static String reportStr;

    if( shouldUpdateThisFrame )
    {
        // Build Report
        Profiler::Measurement* frame = Profiler::GetPreviousFrame( frameSkip );
        ProfilerReport report;

        if( g_hierarchy == DisplayHierarchy::TREE )
            report.GenerateReportTreeFromFrame( frame );
        else if( g_hierarchy == DisplayHierarchy::FLAT )
            report.GenerateReportFlatFromFrame( frame );

        if( g_sortMode == DisplaySortMode::TOTAL )
            report.SortByTotalTime();
        else if( g_sortMode == DisplaySortMode::SELF )
            report.SortBySelfTime();

        reportStr = report.ToString();

        // update FPS and frame time
        timeOfLastUpdate = currentTime;
        frameTime = report.GetTotalFrameTime() * 1000;
        fps = 1 / report.GetTotalFrameTime();
    }

    DebugRender::SetOptions( 0 );

    // Draw Title
    bounds.Translate( 10, -10 );
    DebugRender::DrawText2D( bounds, fontHeight * 2, Vec2( 0, 1 ), "PROFILER" );

    // Draw FPS and frame time
    bounds.Translate( 0, -fontHeight * 2 );
    DebugRender::DrawText2D(
        bounds, fontHeight, Vec2( 0, 1 ),
        "FPS: %.2f \nFrame Time: %.2f ms", fps, frameTime );

    // Draw report
    bounds.Translate( 0, -fontHeight * 10 );
    DebugRender::DrawText2D(
        bounds, fontHeight, Vec2( 0, 1 ),
        reportStr );
}

void Render()
{
    if( !g_visible )
        return;

    Renderer* renderer = Renderer::GetDefault();
    renderer->ClearDepth();
    renderer->UseUICamera();

    Window* window = renderer->GetWindow();
    AABB2 bounds = window->GetWindowBounds();
    renderer->DrawAABB( bounds, Rgba( 0, 0, 0, 200 ) );

    RenderReport();
}

void ProcessInput()
{
    if( !g_visible )
        return;

    InputSystem* input = InputSystem::GetDefault();
    if( input->WasKeyJustPressed( 'V' ) )
    {
        g_hierarchy = g_hierarchy + 1;
        if( g_hierarchy >= DisplayHierarchy::COUNT )
            g_hierarchy = 0;
    }

    if( input->WasKeyJustPressed( 'L' ) )
    {
        g_sortMode = g_sortMode + 1;
        if( g_sortMode >= DisplaySortMode::COUNT )
            g_sortMode = 0;
    }

    if( input->WasKeyJustPressed( 'M' ) )
    {
        if( !g_cursorSettingsPushed )
        {
            input->PushCursorSettings();
            input->ShowAndUnlockCursor();
            g_cursorSettingsPushed = true;
        }
        else
        {
            g_cursorSettingsPushed = false;
            input->PopCursorSettings();
        }
    }
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

void Render() {}

Profiler::Measurement* GetPreviousFrame( int skipCount ) { (void) ( skipCount ); }

void Pause() {}

void Resume() {}

#endif



}