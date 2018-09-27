#pragma once
#include <vector>
#include <string>
#include <map>

#include "Engine/Core/SmartEnum.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Types.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/UBO.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/RendererEnums.hpp"
#include "Engine/Renderer/RenderState.hpp"

class Image;
class Vec2;
class Texture;
class BitmapFont;
class SpriteSheet;
class IVec2;
struct VertexPCU;
class RenderingContext;
class ShaderProgram;
class RenderBuffer;
class Window;
class Mat4;
class Sampler;
class Mesh;
class Camera;
class FrameBuffer;
class Clock;
class Renderable;
class Material;
class Transform;
class ShaderPass;
class VertexLayout;
class DrawCall;
class Light;

// Returns the offset of one alignment to another
Vec2 AlignmentOffsetFromCenter( Alignment alignment );  //range in [-0.5, 0.5]
Vec2 AlignmentOffsetFromReference( Alignment reference, Alignment offsetAlignment ); //range in [-1, 1]

class Renderer
{
    friend RenderingContext;
public:		// Public methods
    static Renderer* GetDefault();
    Renderer( Window* window );
    ~Renderer();

    Window* GetWindow() { return m_window; };
    void Init();  //set line width, blend mode, and so on, TODO move to better place
    void PostInit();
    void BeginFrame();
    void EndFrame();
    void ClearScreen( const Rgba& clearColor );
    void SetBackGroundColor( const Rgba& color );

    void ClearDepth( float depth = 1.0f );


    // Draw Methods
    void DrawMeshImmediate( const VertexPCU* verts, int numVerts,
                            uint* indices = nullptr, int numIndices = 0,
                            DrawPrimitive primitive = DrawPrimitive::TRIANGLES );
    void DrawMeshWithTexture( Mesh* mesh, const Texture* texture = nullptr );
    void DrawUIMeshWithTexture( Mesh* mesh, const Texture* texture = nullptr );
    void DrawRenderable( Renderable* renderable );

    void DrawRenderablePass( Renderable* renderable,
                             uint subMeshID = 0, uint shaderPassID = 0 );

    // Draw Outlines
    void DrawLine( const Vec3& start, const Vec3& end, const Rgba& startColor
                   , const Rgba& endColor, float lineThickness = 1.f );
    // draw line with same start and end color, with 1.f thickness
    void DrawLine( const Vec3& start, const Vec3& end,
                   const Rgba& color = Rgba::WHITE );
    void DrawLines( const Vec3* vertPtrs, int numOfVerts,
                    const Rgba& color = Rgba::WHITE );
    void DrawLineLoop( const Vec3* vertPtrs, int numOfVerts,
                       const Rgba& color = Rgba::WHITE );
    //     void DrawDottedCircle( const Vec3& center, float radius, const Vec3& normal,
    //                            const Rgba& color = Rgba::WHITE, int sides = 30 );
    void DrawRect( const AABB2& bounds, const Rgba& color = Rgba::MAGENTA );
    void DrawPoints( const vector<Vec3>& points, float pointSize = 1.f,
                     const Rgba& color = Rgba::WHITE );

    // Draw Cube
    void DrawCube( const Vec3& centerPos,
                   const Vec3& sideLengths = Vec3::ONES,
                   const Rgba& tint = Rgba::WHITE,
                   const AABB2& uvTop = AABB2::ZEROS_ONES,
                   const AABB2& uvSide = AABB2::ZEROS_ONES,
                   const AABB2& uvBottom = AABB2::ZEROS_ONES );
    // Draw Quads
    void DrawAABB( const AABB2& bounds, const Rgba& color );
    void DrawTexturedAABB(
        const Texture* texture,
        const AABB2& bounds = AABB2::NEG_ONES_ONES,
        const AABB2& uvCoords = AABB2::ZEROS_ONES,
        const Rgba& tint = Rgba::WHITE );

    // Draw Text
    void DrawText2D(
        const string& text,
        const Vec2& position,
        float height,
        const BitmapFont* font = nullptr,
        const Rgba& color = Rgba::WHITE,
        Alignment alignment = Alignment::CENTER_CENTER,
        float aspectScale = 1.f );

    void DrawTextInBox2D(
        const string& text,
        const AABB2& box,
        float fontHeight,
        const BitmapFont* font = nullptr,
        TextDrawMode textDrawMode = TextDrawMode::WORD_WRAP,
        const Vec2& alignment = Vec2::POINT_FIVES,
        const Rgba& color = Rgba::WHITE,
        float aspectScale = 1.f );

    void DrawText2DFlashWave(
        const string& text,
        const Vec2& position,
        float height,
        const BitmapFont* font,
        float time,
        const Rgba& colorA = Rgba::WHITE, // a random m_color between colorA and colorB is picked each cycle
        const Rgba& colorB = Rgba::BLACK,
        float interval = 1.f,
        float flashDuration = 0.1f,
        float nextGlyphOffsetSeconds = 0.01f,
        const Rgba& colorFlash = Rgba::WHITE,
        float bumpUpRatio = 0.1f,
        Alignment alignment = Alignment::CENTER_CENTER,
        float aspectScale = 1.f );


    // Texture creation / destruction
    Texture* CreateOrGetTexture( const string& texturePath ); // Does not reload if texturePath is the same
    Texture* GetTexture( const string& texturePath );
    Texture* CreateOrGetTexture( Image* image ); // Does not automatically cache, so hold on to the texture pointer

    // Render Targets
    Texture* CreateRenderTarget( uint width, uint height, TextureFormat format = TextureFormat::RGBA8 );
    Texture* CreateDepthStencilTarget( uint width, uint height );
    Texture* DefaultColorTarget() { return m_defaultColorTarget; };
    Texture* DefaultDepthTarget() { return m_defaultDepthTarget; };
    Texture* DefaultShadowTarget() { return m_defaultShadowTarget; };
    void TakeScreenshot();

    bool CopyFrameBuffer( FrameBuffer* dst, FrameBuffer* src );

    void SetDefaultFont( const string& bitmapFontPath );
    BitmapFont* CreateOrGetBitmapFont( const string& bitmapFontPath );
    BitmapFont* GetBitmapFont( const string& bitmapFontPath );
    BitmapFont* DefaultFont() { return m_defaultFont; };

    SpriteSheet* CreateOrGetSpriteSheet( const string& spriteSheetPath, const IVec2& layout );
    SpriteSheet* GetSpriteSheet( const string& spriteSheetPath );

    void DeleteTexture( Texture*& texture );

    //Camera
    void UseCamera( Camera* camera );
    void UseMainCamera();
    void UseUICamera();
    void SetMainCamera( Camera* camera );

    Camera* GetUICamera() { return m_UICamera; };
    Camera* GetShadowCamera() { return m_shadowCamera; };
    Camera* GetMainCamera() { return m_mainCamera; };


    //Sampler
    void BindSampler( uint textureUnitIdx, Sampler* sampler );

    void BindTexture( uint textureUnitIdx, const Texture* texture );

    //void CopyTextureOnGPU( Texture* source, Texture* target );

    void ApplyEffect( Material* material );


    // Time
    void SetClocks( Clock* gameClock, Clock* appClock );
    void UpdateAndBindTimeUBO();


    //  Renderable Hierarchy
    //
    //      Mesh
    //          VertexBuffer
    //          IndexBuffer
    //          vector<DrawInstruction>
    //
    //      ModelMatrix
    //
    //      vector<Material>
    //          Textures + Samplers
    //          Properties
    //          Shader
    //              ShaderPass
    //                  ShaderProgram
    //                  RenderState



    void SetAmbient( const Vec4& color );

    // use cos( theta / 2 ) for the inner and outer dot, where theta is the cone angle
    void SetLight( uint idx, const Vec4& color, const Vec3& position,
                   float sourceRadius = 1,
                   float isPointLight = 1,
                   const Vec3& direction = Vec3( 1, 0, 0 ),
                   float coneInnerDot = -2, float coneOuterDot = -2,
                   bool usesShadow = false );

    void SetLight( uint idx, Light* light );

    void SetPointLight( uint idx, const Vec4& color, const Vec3& position,
                        float sourceRadius = 1 );

    void SetDirectionalLight( uint idx, const Vec4& color, const Vec3& position,
                              const Vec3& direction = Vec3( 1, 0, 0 ) );

    void SetSpotLight( uint idx, const Vec4& color, const Vec3& position,
                       float sourceRadius = 1,
                       const Vec3& direction = Vec3( 1, 0, 0 ),
                       float coneInnerDot = -2, float coneOuterDot = -2 );

    void DisableAllLights();


    void BindRenderState( const RenderState& renderState, bool forced = false );

    // Debug
    void SetDebugWireframeShader();
    void SetDebugLightingShader();

    // This shader will override all shaders
    // Set to null to turn off override mode
    void SetOverrideShader( ShaderPass* shader ) { m_overrideShader = shader; };

    void SetWindowUBO( Vec2 windowSize );

    void SetFog( const Rgba& color, float nearPlane, float nearFactor,
                 float farPlane, float farFactor );

    void SetShadowMapVP( const Mat4& vp );
    void BindShadowTextureAsInput( bool bind );

private:	// Private members

    RenderState & GetCurrentRenderState();

    // RenderState
    void SetCullModeGL( CullMode cull, bool forced = false );
    void SetFillModeGL( FillMode fill, bool forced = false );
    void SetWindOrderGL( WindOrder windOrder, bool forced = false );
    void EnableBlendingGL( BlendOP op, BlendFactor src, BlendFactor dst, bool forced = false );
    void DisableBlendingGL( bool forced = false );
    void EnableDepthGL( DepthCompareMode compare = DepthCompareMode::LESS,
                        bool shouldWrite = true, bool forced = false );
    void DisableDepthGL( bool forced = false );
    void SetCurrentRenderState( const RenderState& state );

    // return programHandle
    uint BindShader( ShaderPass* shader );
    void BindInstanceUBO( const Mat4& model, const Material* material );
    void BindLightUBO();

    map<string, Texture*> m_loadedTextures;
    map< string, BitmapFont* > m_loadedFonts;
    map< string, SpriteSheet* > m_loadedSpriteSheets;

    Rgba m_backgroundColor = Rgba::DIM_GRAY;

    BitmapFont* m_defaultFont = nullptr;

    Texture* m_defaultColorTarget = nullptr;
    Texture* m_defaultDepthTarget = nullptr;
    Texture* m_effectColorTarget = nullptr;
    Texture* m_defaultShadowTarget = nullptr;

    Camera* m_currentCamera = nullptr;
    Camera* m_mainCamera = nullptr;
    Camera* m_UICamera = nullptr;
    Camera* m_effectCamera = nullptr;
    Camera* m_shadowCamera = nullptr;

    RenderState m_currentRenderState;

    // Window
    Window* m_window = nullptr;
    // GL context
    RenderingContext* m_renderingContext = nullptr;

    // UBOs
    UBO::TimeData m_timeUBOData;
    UniformBuffer m_timeBuffer;
    UBO::CameraData m_cameraUBOData;
    UniformBuffer m_cameraUniformBuffer;
    UBO::InstanceData m_instanceUBOData;
    UniformBuffer m_instanceUniformBuffer;
    UBO::LightsData m_lightUBOData;
    UniformBuffer m_lightUniformBuffer;
    UBO::GlobalData m_globalUBOData;
    UniformBuffer m_globalUniformBuffer;

    // Clock
    Clock* m_gameClock = nullptr;
    Clock* m_appClock = nullptr;

    // Debug and overriding effects shaders, depth shader
    ShaderPass* m_overrideShader = nullptr;
};


