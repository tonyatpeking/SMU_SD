#include <stdarg.h>
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/MeshPrimitive.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/TextMeshBuilder.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Profiler/Profiler.hpp"

namespace
{
const float LOG_TEXT_HEIGHT = 15;
const int LOG_MAX_SLOTS = 50;
const Rgba XRAY_TINT = Rgba( 255, 0, 0, 190 );

void Render3DCommands();
void Render2DCommands();
uint AddTask( Renderable* renderable );
void RemoveLogFromSlot( uint handle );
Renderable* MakeRenderable3D();

// returns -1 if none
int GetNextEmptyLogSlot();

// returns the screen ui space aabb2 for that log slot
AABB2 LogSlotToScreenBounds( int slot );


// -1 is empty slot
Ints g_logSlots( LOG_MAX_SLOTS, -1 );

std::vector < DebugRender::Task* > g_tasks;
Camera* g_2dCamera;
Camera* g_3dCamera;
DebugRender::Options g_options;
Renderer* g_renderer;
Clock* g_clock;
bool g_hide = false;

uint g_taskUidCounter = 0;

void Render3DCommands()
{
    g_renderer->UseCamera( g_3dCamera );
    for( auto& task : g_tasks )
    {
        if( !task->m_options.m_screenspace )
        {
            task->Render();
            if( task->m_options.m_mode == DebugRender::Mode::XRAY )
            {
                Rgba savedTint = task->m_renderable->GetMaterial( 0 )->m_tint;
                task->m_renderable->GetMaterial( 0 )->m_tint = XRAY_TINT;
                task->m_renderable->GetMaterial( 0 )->m_shaderPass->EnableDepth(
                    DepthCompareMode::GREATER, false );
                task->Render();

                task->m_renderable->GetMaterial( 0 )->m_tint = savedTint;
                task->m_renderable->GetMaterial( 0 )->m_shaderPass->EnableDepth(
                    DepthCompareMode::LEQUAL );
            }
        }
    }
}

void Render2DCommands()
{
    g_renderer->UseCamera( g_2dCamera );
    for( auto& task : g_tasks )
    {
        if( task->m_options.m_screenspace )
            task->Render();
    }
}

uint AddTask( Renderable* renderable )
{
    DebugRender::Task* task = new DebugRender::Task();
    task->m_options = g_options;
    task->m_timeToLive = g_options.m_lifetime;
    task->m_renderable = renderable;
    g_tasks.push_back( task );
    return task->m_uid;
}

void RemoveLogFromSlot( uint handle )
{
    for( int slot = 0; slot < g_logSlots.size(); ++slot )
    {
        if( g_logSlots[slot] == (int) handle )
        {
            g_logSlots[slot] = -1;
            return;
        }
    }
}

int GetNextEmptyLogSlot()
{
    for( int slot = 0; slot < g_logSlots.size(); ++slot )
    {
        if( g_logSlots[slot] == -1 )
        {
            return slot;
        }
    }
    return -1;
}

AABB2 LogSlotToScreenBounds( int slot )
{
    Window* window = g_renderer->GetWindow();
    Vec2 dim = window->GetDimensions();
    AABB2 bounds = AABB2( 0, -LOG_TEXT_HEIGHT, dim.x, 0 );
    bounds.Translate( 0, dim.y );
    bounds.Translate( 0, -slot * LOG_TEXT_HEIGHT );
    return bounds;
}

Renderable* MakeRenderable3D()
{
    g_options.m_screenspace = false;
    Renderable* renderable = new Renderable();
    renderable->GetMaterial( 0 )->SetTint( g_options.m_startColor );
    ShaderPass* shader = new ShaderPass();
    renderable->GetMaterial( 0 )->m_shaderPass = shader;
    shader->SetFillMode( g_options.m_fillmode );
    switch( g_options.m_mode )
    {
    case DebugRender::Mode::IGNORE_DEPTH:
        shader->DisableDepth();
        break;
    case DebugRender::Mode::USE_DEPTH:
        shader->EnableDepth();
        break;
    case DebugRender::Mode::HIDDEN:
        shader->EnableDepth( DepthCompareMode::GREATER, false );
        break;
    case DebugRender::Mode::XRAY:
        // do a second pass during rendering that ignores depth
        shader->EnableDepth( DepthCompareMode::LEQUAL );
        break;
    default:
        break;
    }

    return renderable;
}

}
//--------------------------------------------------------------------------------------
// End unnamed namespace
//--------------------------------------------------------------------------------------

void DebugRender::Startup( Renderer *renderer )
{
    g_renderer = renderer;
    g_3dCamera = renderer->GetMainCamera();
    g_2dCamera = renderer->GetUICamera();
}

void DebugRender::Shutdown()
{
    Clear();
}

void DebugRender::UpdateAndRender()
{
    PROFILER_SCOPED();
    for( auto& task : g_tasks )
        task->Age();

    if( !g_hide )
    {
        Render3DCommands();
        Render2DCommands();
    }

    for( int taskIdx = (int) g_tasks.size() - 1; taskIdx >= 0; --taskIdx )
    {
        Task* task = g_tasks[taskIdx];
        if( task->ShouldDie() )
        {
            delete task;
            ContainerUtils::EraseAtIndexFast( g_tasks, taskIdx );
        }
    }
}

void DebugRender::SetHidden( bool hide )
{
    g_hide = hide;
}

void DebugRender::Clear()
{
    ContainerUtils::DeletePointers( g_tasks );
}

DebugRender::Task* DebugRender::GetTask( uint handle )
{
    for( auto& task : g_tasks )
    {
        if( task->m_uid == handle )
            return task;
    }
    return nullptr;
}

void DebugRender::KillTask( uint handle )
{
    Task* task = GetTask( handle );
    if( task )
        task->Kill();
}

int DebugRender::GetTaskCount()
{
    return (int) g_tasks.size();
}

DebugRender::Options& DebugRender::GetOptions()
{
    return g_options;
}

void DebugRender::SetOptions( float lifetime, const Rgba& startColor /*= Rgba::WHITE*/,
                              const Rgba& endColor /*= Rgba::WHITE*/,
                              Mode mode /*= Mode::USE_DEPTH */,
                              FillMode fillmode )
{
    g_options.m_lifetime = lifetime;
    g_options.m_startColor = startColor;
    g_options.m_endColor = endColor;
    if( lifetime == 0.f )
        g_options.m_endColor = startColor;
    g_options.m_mode = mode;
    g_options.m_fillmode = fillmode;
}

void DebugRender::Set3DCamera( Camera* cam )
{
    g_3dCamera = cam;
}

void DebugRender::SetClock( Clock* clock )
{
    g_clock = clock;
}

uint DebugRender::DrawQuad2D( const AABB2& bounds, Texture* texture )
{
    g_options.m_screenspace = true;
    Renderable* renderable = new Renderable();
    renderable->GetMaterial( 0 )->SetTint( g_options.m_startColor );
    renderable->GetMaterial( 0 )->SetDiffuse( texture );
    renderable->SetMesh( MeshPrimitive::MakeQuad( bounds ).MakeMesh() );
    renderable->GetMaterial( 0 )->m_shaderPass = ShaderPass::GetDefaultUIShader();
    return AddTask( renderable );
}

uint DebugRender::DrawLine2D( const Vec2& p0, const Vec2& p1, const Rgba& colorP0,
                              const Rgba& colorP1 )
{
    g_options.m_screenspace = true;
    Renderable* renderable = new Renderable();
    renderable->GetMaterial( 0 )->SetTint( g_options.m_startColor );
    std::vector<Vec3> points;
    points.push_back( Vec3( p0 ) );
    points.push_back( Vec3( p1 ) );
    renderable->SetMesh( MeshPrimitive::MakeLineStrip( points, colorP0, colorP1 ).MakeMesh() );
    renderable->GetMaterial( 0 )->m_shaderPass = ShaderPass::GetDefaultUIShader();
    return AddTask( renderable );
}

uint DebugRender::DrawText2D( const AABB2& bounds, float fontSize, const Vec2& alignment,
                              const char* format, ... )
{

    va_list args;
    va_start( args, format );
    String text = Stringf( format, args );
    uint id = DrawText2D( bounds, fontSize, alignment, text );
    return id;
}



uint DebugRender::DrawText2D( const AABB2& bounds, float fontSize, const Vec2& alignment, const String& str )
{
    g_options.m_screenspace = true;
    Renderable* renderable = new Renderable();
    renderable->GetMaterial( 0 )->SetTint( g_options.m_startColor );
    TextMeshBuilder tmb{};
    tmb.m_boundingBox = bounds;
    tmb.m_fontHeight = fontSize;
    tmb.m_alignment = alignment;
    tmb.m_text = str;
    tmb.Finalize();
    renderable->SetMesh(tmb.MakeMesh());
    renderable->GetMaterial( 0 )->m_diffuse = tmb.m_font->GetTexture();
    renderable->GetMaterial( 0 )->m_shaderPass = ShaderPass::GetDefaultUIShader();
    return AddTask( renderable );
}

uint DebugRender::Logf( const char* format, ... )
{
    g_options.m_screenspace = true;
    int slot = GetNextEmptyLogSlot();
    if( -1 == slot )
        return (uint) -1;
    AABB2 bounds = LogSlotToScreenBounds( slot );
    va_list args;
    va_start( args, format );
    String text = Stringf( format, args );
    uint handle = DrawText2D( bounds, LOG_TEXT_HEIGHT, Vec2::ZERO_ONE, text.c_str() );
    g_logSlots[slot] = handle;
    GetTask( handle )->m_onDeathFunc = [handle]() {RemoveLogFromSlot( handle ); };
    return handle;
}


uint DebugRender::DrawPoint( const Vec3& pos, float size )
{
    g_options.m_screenspace = false;
    Renderable* renderable = new Renderable();
    renderable->GetMaterial( 0 )->SetTint( g_options.m_startColor );
    MeshBuilder mb{};
    mb.BeginSubMesh( DrawPrimitive::LINES, false );
    mb.PushPos( pos + Vec3::FORWARD * size );
    mb.PushPos( pos + Vec3::BACKWARD * size );
    mb.PushPos( pos + Vec3::LEFT * size );
    mb.PushPos( pos + Vec3::RIGHT * size );
    mb.PushPos( pos + Vec3::UP * size );
    mb.PushPos( pos + Vec3::DOWN * size );
    mb.EndSubMesh();
    renderable->SetMesh( mb.MakeMesh());
    return AddTask( renderable );

}

uint DebugRender::DrawLine( const Vec3& p0, const Vec3& p1, const Rgba& color0, const Rgba& color1 )
{
    g_options.m_screenspace = false;
    Renderable* renderable = new Renderable();
    renderable->GetMaterial( 0 )->SetTint( g_options.m_startColor );
    std::vector<Vec3> points;
    points.push_back( p0 );
    points.push_back( p1 );
    renderable->SetMesh( MeshPrimitive::MakeLineStrip(
        points, color0, color1 ).MakeMesh());
    return AddTask( renderable );
}

uint DebugRender::DrawBasis( const Mat4& basis )
{
    g_options.m_screenspace = false;
    Renderable* renderable = new Renderable();
    renderable->GetMaterial( 0 )->SetTint( g_options.m_startColor );


    MeshBuilder mb{};
    mb.BeginSubMesh( DrawPrimitive::LINES, false );

    mb.SetColor( Rgba::RED );
    mb.PushPos( Vec3::ZEROS );
    mb.PushPos( Vec3::RIGHT );

    mb.SetColor( Rgba::GREEN );
    mb.PushPos( Vec3::ZEROS );
    mb.PushPos( Vec3::UP );

    mb.SetColor( Rgba::BLUE );
    mb.PushPos( Vec3::ZEROS );
    mb.PushPos( Vec3::FORWARD );

    mb.EndSubMesh();
    renderable->SetMesh( mb.MakeMesh());
    renderable->m_modelMatrix = basis;


    //     std::function<void( void )> updatePos = [renderable]()
    //     {
    //         Vec3 pos = g_3dCamera->ScreenToWorld( Vec2( 100, 100 ), 1 );
    //         renderable->m_modelMatrix.T = Vec4( pos, 1 );
    //     };
    //     updatePos();
    uint handle = AddTask( renderable );
    //GetTask( handle )->m_onUpdateFunc = updatePos;

    return handle;
}

uint DebugRender::DrawSphere( const Vec3& pos, float radius /*= 1 */ )
{
    Renderable* renderable = MakeRenderable3D();
    renderable->SetMesh( MeshPrimitive::MakeUVSphere( radius ).MakeMesh());
    renderable->SetModelMatrix( Mat4::MakeTranslation( pos ) );
    return AddTask( renderable );
}

uint DebugRender::DrawAABB3( const AABB3& bounds )
{
    Renderable* renderable = MakeRenderable3D();
    renderable->SetMesh( MeshPrimitive::MakeCube( bounds.GetDimensions() ).MakeMesh());
    renderable->SetModelMatrix( Mat4::MakeTranslation( bounds.GetCenter() ) );
    return AddTask( renderable );
}

uint DebugRender::DrawOBB3( const OBB3& obb3 )
{
    Renderable* renderable = MakeRenderable3D();
    renderable->SetMesh( MeshPrimitive::MakeCube(
        obb3.GetAABB3().GetDimensions() ).MakeMesh());
    renderable->SetModelMatrix( obb3.GetLocalToWorld() );
    return AddTask( renderable );
}

uint DebugRender::DrawQuad( const AABB2& bounds, const Vec3& pos, const Vec3& euler,
                            Texture* texture /*= nullptr*/ )
{
    Renderable* renderable = MakeRenderable3D();
    renderable->SetMesh( MeshPrimitive::MakeQuad( bounds ).MakeMesh());
    renderable->GetMaterial( 0 )->m_diffuse = texture;
    renderable->SetModelMatrix( Mat4::MakeFromSRT( Vec3::ONES, euler, pos ) );
    return AddTask( renderable );

}

uint DebugRender::DrawGrid( const Vec3& center, float gridSize, float maxLength )
{
    g_options.m_screenspace = false;
    Renderable* renderable = new Renderable();

    MeshBuilder mb{};
    mb.BeginSubMesh( DrawPrimitive::LINES, false );

    // find mins that is snapped to grid
    Vec3 mins = center - Vec3::ONES * maxLength / 2;
    mins = SnapToMultiplesOf( mins, gridSize );
    Vec3 maxs = center + Vec3::ONES * maxLength / 2;
    maxs = SnapToMultiplesOf( maxs, gridSize );

    // X axis
    mb.SetColor( Rgba::RED );
    for( float z = mins.z; z <= maxs.z; z += gridSize )
    {
        for( float y = mins.y; y <= maxs.y; y += gridSize )
        {
            mb.PushPos( Vec3( mins.x, y, z ) );
            mb.PushPos( Vec3( maxs.x, y, z ) );
        }
    }

    // Y axis
    mb.SetColor( Rgba::GREEN );
    for( float z = mins.z; z <= maxs.z; z += gridSize )
    {
        for( float x = mins.x; x <= maxs.x; x += gridSize )
        {
            mb.PushPos( Vec3( x, mins.y, z ) );
            mb.PushPos( Vec3( x, maxs.y, z ) );
        }
    }

    // Z axis
    mb.SetColor( Rgba::BLUE );
    for( float y = mins.y; y <= maxs.y; y += gridSize )
    {
        for( float x = mins.x; x <= maxs.x; x += gridSize )
        {
            mb.PushPos( Vec3( x, y, mins.z ) );
            mb.PushPos( Vec3( x, y, maxs.z ) );
        }
    }

    mb.EndSubMesh();
    renderable->SetMesh( mb.MakeMesh());

    uint handle = AddTask( renderable );

    return handle;
}

uint DebugRender::DrawText( const AABB2& bounds, const Mat4& transform, float fontSize,
                            const Vec2& alignment, const char* format, ... )
{
    g_options.m_screenspace = false;
    Renderable* renderable = new Renderable();
    renderable->GetMaterial( 0 )->SetTint( g_options.m_startColor );
    TextMeshBuilder tmb{};
    tmb.m_boundingBox = bounds;
    tmb.m_fontHeight = fontSize;
    tmb.m_alignment = alignment;
    va_list args;
    va_start( args, format );
    tmb.m_text = Stringf( format, args );
    tmb.Finalize();
    renderable->SetMesh( tmb.MakeMesh());
    renderable->GetMaterial( 0 )->m_diffuse = tmb.m_font->GetTexture();
    renderable->GetMaterial( 0 )->m_shaderPass = new ShaderPass();
    renderable->GetMaterial( 0 )->m_shaderPass->EnableDepth( DepthCompareMode::LESS, false );
    renderable->GetMaterial( 0 )->m_shaderPass->EnableBlending( BlendMode::ALPHA_BLEND );
    renderable->m_modelMatrix = transform;
    return AddTask( renderable );
    return 0;

}

// uint DebugRender::DrawBillboardText( const AABB2& bounds, float fontSize, const Vec2& alignment, const char* format, ... )
// {
//     g_options.m_screenspace = false;
//     return 0;
//
// }
//
// uint DebugRender::DrawBillboard( const AABB2& bounds, Texture* texture /*= nullptr */ )
// {
//     g_options.m_screenspace = false;
//     return 0;
//
// }
//
uint DebugRender::DrawTag( const AABB2& bounds, const Vec3& pos, float fontSize,
                           const Vec2& alignment, const char* format, ... )
{
    g_options.m_screenspace = true;
    Renderable* renderable = new Renderable();
    renderable->GetMaterial( 0 )->SetTint( g_options.m_startColor );
    TextMeshBuilder tmb{};
    tmb.m_boundingBox = bounds;
    tmb.m_fontHeight = fontSize;
    tmb.m_alignment = alignment;
    va_list args;
    va_start( args, format );
    tmb.m_text = Stringf( format, args );
    tmb.Finalize();
    renderable->SetMesh( tmb.MakeMesh());
    renderable->GetMaterial( 0 )->m_diffuse = tmb.m_font->GetTexture();
    renderable->GetMaterial( 0 )->m_shaderPass = ShaderPass::GetDefaultUIShader();
    uint handle = AddTask( renderable );

    std::function<void( void )> updatePos = [renderable, pos]()
    {
        Vec2 screenPos = g_3dCamera->WorldToScreenInvertY( pos );

        renderable->m_modelMatrix.T = Vec4( screenPos, 0, 1 );
    };

    GetTask( handle )->m_onUpdateFunc = updatePos;

    return handle;
}



uint DebugRender::DrawGlyph( const AABB2& bounds, const Vec3& pos, Texture* texture /*= nullptr */ )
{
    g_options.m_screenspace = true;
    Renderable* renderable = new Renderable();
    renderable->SetMesh( MeshPrimitive::MakeQuad( bounds ).MakeMesh());
    renderable->GetMaterial( 0 )->SetTint( g_options.m_startColor );
    renderable->GetMaterial( 0 )->m_diffuse = texture;
    renderable->GetMaterial( 0 )->m_shaderPass = ShaderPass::GetDefaultUIShader();
    uint handle = AddTask( renderable );

    std::function<void( void )> updatePos = [renderable, pos]()
    {
        Vec2 screenPos = g_3dCamera->WorldToScreenInvertY( pos );

        renderable->m_modelMatrix.T = Vec4( screenPos, 0, 1 );
    };

    GetTask( handle )->m_onUpdateFunc = updatePos;

    return handle;
}

DebugRender::Task::Task()
    : m_uid( ++g_taskUidCounter )
{
}

DebugRender::Task::~Task()
{
    if( m_onDeathFunc )
        m_onDeathFunc();
    m_renderable->DeleteMesh();
    delete m_renderable;
}

void DebugRender::Task::Render()
{
    if( m_visible )
        g_renderer->DrawRenderable( m_renderable );
}

void DebugRender::Task::Age()
{
    if( m_onUpdateFunc )
        m_onUpdateFunc();
    m_timeToLive -= g_clock->GetDeltaSecondsF();

    float normalizedAge = GetNormalizedAge();
    Rgba tint = Lerp( m_options.m_startColor, m_options.m_endColor, normalizedAge );
    m_renderable->GetMaterial( 0 )->SetTint( tint );
}

void DebugRender::Task::Kill()
{
    m_options.m_lifetime = 0;
    m_timeToLive = 0;
}

bool DebugRender::Task::ShouldDie()
{
    if( m_options.m_lifetime == -1 )
        return false;

    return m_timeToLive <= 0;
}

float DebugRender::Task::GetNormalizedAge()
{
    float lifeTime = m_options.m_lifetime;
    if( lifeTime == -1 )
        return 0;

    float percentAlive = m_timeToLive / lifeTime;
    float percentDead = 1.f - percentAlive;
    return Clampf01( percentDead );
}

