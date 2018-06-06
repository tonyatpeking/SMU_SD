#pragma once
#include "Engine/Core/SmartEnum.hpp"
#include "Engine/Core/Types.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat4.hpp"
#include <functional>
#include "Engine/Renderer/RendererEnums.hpp"

class Renderer;
class Camera;
class Clock;
class AABB3;
class AABB2;
class Vec2;
class Texture;
class Renderable;

namespace DebugRender
{


SMART_ENUM(
    Mode,

    IGNORE_DEPTH,
    USE_DEPTH,
    HIDDEN,
    XRAY
)

struct Options
{
    Options() {};

    // these are the start of life and end of life colors
    Rgba m_startColor = Rgba::WHITE;
    Rgba m_endColor = Rgba::WHITE;
    // 0 for one frame, -1 for indefinite
    float m_lifetime = 1.f;
    Mode m_mode = Mode::USE_DEPTH;
    // do not set mannually, this is set by the differen draw functions
    bool m_screenspace = false;
    FillMode m_fillmode = FillMode::SOLID;
};

const Options DEFAULT_OPTIONS = Options{};

class Task
{
public:
    Task();
    ~Task();

    void Render();
    void Age();
    void Kill();
    void SetVisible( bool visible ) { m_visible = visible; };
    bool ShouldDie();
    // 0 when just created, 1 when dead.
    float GetNormalizedAge();

public:
    std::function<void( void )> m_onDeathFunc = nullptr;
    std::function<void( void )> m_onUpdateFunc = nullptr;
    uint m_uid = 0;
    bool m_visible = true;
    Options m_options;
    Renderable* m_renderable;
    float m_timeToLive;
};


void Startup( Renderer *renderer );
void Set3DCamera( Camera* cam );
void SetClock( Clock* clock );
void Shutdown();

void UpdateAndRender();
void SetHidden( bool hide );
void Clear();

// will return null if task has been destroyed
Task* GetTask( uint handle );
void KillTask( uint handle );

// set the options will affect future pushed commands
Options& GetOptions();
void SetOptions( float lifetime, const Rgba& startColor = Rgba::WHITE,
                 const Rgba& endColor = Rgba::WHITE, Mode mode = Mode::USE_DEPTH,
                 FillMode fillmode = FillMode::SOLID );



// 2D
uint DrawQuad2D( const AABB2& bounds, Texture* texture = nullptr );
uint DrawLine2D( const Vec2& p0, const Vec2& p1,
                 const Rgba& colorP0 = Rgba::WHITE, const Rgba& colorP1 = Rgba::WHITE );
uint DrawText2D( const AABB2& bounds, float fontSize, const Vec2& alignment,
                 const char* format, ... );
uint Logf( const char* format, ... );
// text that always faces camera, and is always the same size,
// pos is world pos but draws in screen space
uint DrawTag( const AABB2& bounds, const Vec3& pos, float fontSize,
              const Vec2& alignment, const char* format, ... );
// Similar to draw tag but draws a texture
uint DrawGlyph( const AABB2& bounds, const Vec3& pos, Texture* texture = nullptr );

// 3D
uint DrawPoint( const Vec3& pos, float size = 0.1f );
uint DrawLine( const Vec3& p0, const Vec3& p1, const Rgba& color0, const Rgba& color1 );
uint DrawBasis( const Mat4& basis = Mat4::IDENTITY );
uint DrawSphere( const Vec3& pos, float radius = 1 );
uint DrawAABB3( const AABB3& bounds );
uint DrawQuad( const AABB2& bounds, const Vec3& pos, const Vec3& euler = Vec3::ZEROS,
               Texture* texture = nullptr );
uint DrawGrid( const Vec3& center, float gridSize = 1, float maxLength = 10 );
uint DrawText( const AABB2& bounds, const Mat4& transform, float fontSize,
               const Vec2& alignment, const char* format, ... );
uint DrawBillboardText( const AABB2& bounds, float fontSize,
                        const Vec2& alignment, const char* format, ... );
uint DrawBillboard( const AABB2& bounds, Texture* texture = nullptr );
}

