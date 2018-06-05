#include <vector>

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RenderingContext.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/BuiltinShader.hpp"
#include "Engine/Renderer/FrameBuffer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/MeshPrimitive.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Core/Types.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Vertex.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Renderer/VertexLayout.hpp"
#include "Engine/Renderer/TextureBindings.hpp"
#include "Engine/Renderer/DrawCall.hpp"
#include "Engine/Renderer/Light.hpp"



Renderer::Renderer( Window* window )
    : m_window( window )
{
    Init();
    PostInit();
}

Renderer::~Renderer()
{
    for( auto& pathTexturePair : m_loadedTextures )
        delete pathTexturePair.second;

    delete m_renderingContext;
    delete m_pointSampler;
}

void Renderer::Init()
{
    // check if we should fire up directx or opengl
    m_renderingContext = new RenderingContext( m_window );
    GL_CHECK_ERROR();
}

void Renderer::PostInit()
{
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_LINE_SMOOTH );
    BindRenderState( m_currentRenderState, true );
    //     // m_defaultVAO is a GLuint member variable
    //     glGenVertexArrays( 1, &m_defaultVAO );
    //     glBindVertexArray( m_defaultVAO );


    GL_CHECK_ERROR();

    m_pointSampler = new Sampler();


    // the default m_color and depth should match our output window
    // so get width/height however you need to.
    uint windowWidth = (uint) m_window->GetDimensions().x;
    uint windowHeight = (uint) m_window->GetDimensions().y;
    GL_CHECK_ERROR();

    // init default m_color/depth/effect targets
    m_defaultColorTarget = CreateRenderTarget( windowWidth, windowHeight );
    m_effectColorTarget = CreateRenderTarget( windowWidth, windowHeight );
    m_defaultDepthTarget = CreateDepthStencilTarget( windowWidth, windowHeight );

    // init default camera
    m_defaultCamera  = new Camera();
    m_defaultCamera->SetColorTarget( m_defaultColorTarget );
    m_defaultCamera->SetDepthStencilTarget( m_defaultDepthTarget );
    UseCamera( nullptr );

    // init ui camera
    m_UICamera = new Camera();
    m_UICamera->SetColorTarget( DefaultColorTarget() );
    m_UICamera->SetDepthStencilTarget( DefaultDepthTarget() );
    m_UICamera->SetProjectionOrtho( AABB2( Vec2::ZEROS, (Vec2) m_window->GetDimensions() ), 0.f, 1.f );

    // init effect camera
    m_effectCamera = new Camera();
    m_effectCamera->SetColorTarget( m_effectColorTarget );

}

void Renderer::BeginFrame()
{
    SetWindowUBO( m_window->GetDimensions() );
    ClearScreen( m_backgroundColor );
    ClearDepth();
    //ClearImmediateRenderable();
    UpdateAndBindTimeUBO();
    UseDefaultCamera();
    // Disable vao attrib pointer
    GL_CHECK_ERROR();
}

void Renderer::EndFrame()
{
    GL_CHECK_ERROR();
    // copies the default camera's framebuffer to the "null" framebuffer,
    // also known as the back buffer.
    CopyFrameBuffer( nullptr, m_currentCamera->m_frameBuffer );
    HWND hWnd = GetActiveWindow();
    HDC hDC = GetDC( hWnd );
    SwapBuffers( hDC );

}

void Renderer::ClearScreen( const Rgba& clearColor )
{
    float r;
    float g;
    float b;
    float a;
    clearColor.GetAsFloats( r, g, b, a );
    glBindFramebuffer( GL_FRAMEBUFFER, m_currentCamera->GetFrameBufferHandle() );
    glClearColor( r, g, b, a );
    glClear( GL_COLOR_BUFFER_BIT );
}

void Renderer::SetBackGroundColor( const Rgba & color )
{
    m_backgroundColor = color;
}

//
// void Renderer::SetCullMode( CullMode cull )
// {
//     GetCurrentRenderState().m_cullMode = cull;
// }
//
// void Renderer::SetFillMode( FillMode fill )
// {
//     GetCurrentRenderState().m_fillMode = fill;
// }
//
// void Renderer::SetWindOrder( WindOrder windOrder )
// {
//     GetCurrentRenderState().m_windOrder = windOrder;
// }

void Renderer::DrawMeshImmediate(
    const VertexPCU* verts, int numVerts, uint* indices /*= nullptr*/,
    int numIndices /*= 0*/, DrawPrimitive primitive /*=TRIANGLES */ )
{
    Mesh temp{};
    DrawInstruction drawInstruct{};
    drawInstruct.m_drawPrimitive = primitive;
    temp.m_vertexLayout = &VertexPCU::s_vertexLayout;
    if( nullptr == indices )
    {
        drawInstruct.m_useIndices = false;
        drawInstruct.m_elemCount = numVerts;
    }
    else
    {
        temp.SetIndices( numIndices, indices );
        drawInstruct.m_elemCount = numIndices;
    }
    temp.m_subMeshInstuct.push_back( drawInstruct );
    temp.SetVertices( numVerts, verts );

    DrawMeshWithTexture( &temp );
}

void Renderer::DrawMeshWithTexture( Mesh* mesh, const Texture* texture )
{
    Renderable renderable{};
    renderable.SetMesh( mesh );
    renderable.GetMaterial( 0 )->SetDiffuse( texture );
    DrawRenderablePass( &renderable );
}


void Renderer::DrawRenderablePass( Renderable* renderable, uint subMeshID /*= 0*/,
                                   uint shaderPassID /*= 0 */ )
{
    GL_CHECK_ERROR();

    Mesh* mesh = renderable->GetMesh();
    if( !mesh )
    {
        LOG_WARNING( "Trying to render renderable without mesh!" );
        return;
    }

    renderable->CreateInputLayoutsIfDirty();


    Material* mat = renderable->GetMaterial( subMeshID );
    Mat4& modelMatrix = renderable->GetModelMatrix();

    const Texture* diffuseTexture = nullptr;
    const Texture* normalTextue = nullptr;
    ShaderPass* shaderPass = nullptr;
    DrawInstruction& ins = mesh->GetSubMeshInstruction( subMeshID );

    if( mat )
    {
        diffuseTexture = mat->m_diffuse;
        normalTextue = mat->m_normal;
        shaderPass = mat->GetShaderPass( shaderPassID );
    }
    if( !diffuseTexture )
        diffuseTexture = Texture::GetWhiteTexture();
    if( !normalTextue )
        normalTextue = Texture::GetFlatNormalTexture();

    GL_CHECK_ERROR();
    BindTexture( TextureBindings::DIFFUSE, diffuseTexture );
    BindSampler( TextureBindings::DIFFUSE, m_pointSampler );
    GL_CHECK_ERROR();

    BindTexture( TextureBindings::NORMAL, normalTextue );
    BindSampler( TextureBindings::NORMAL, m_pointSampler );

    BindInstanceUBO( modelMatrix, mat );
    BindLightUBO();

    if( m_debugShader )
        shaderPass = m_debugShader;

    BindShader( shaderPass );

    renderable->BindInputLayoutForProgram( shaderPass->GetProgram() );

    if( ins.m_useIndices )
    {
        glDrawElements( ToGLEnum( ins.m_drawPrimitive ), ins.m_elemCount,
                        GL_UNSIGNED_INT, (void*) ( ins.m_startIdx * sizeof( GLuint ) ) );
    }
    else
    {
        glDrawArrays( ToGLEnum( ins.m_drawPrimitive ), ins.m_startIdx,
                      ins.m_elemCount );
    }
    glBindVertexArray( NULL );
    GL_CHECK_ERROR();
}

void Renderer::DrawRenderable( Renderable* renderable )
{
    DrawRenderablePass( renderable );
}


void Renderer::BindRenderState( const RenderState& rs, bool forced )
{
    SetCullModeGL( rs.m_cullMode, forced );
    SetFillModeGL( rs.m_fillMode, forced );
    SetWindOrderGL( rs.m_windOrder, forced );
    if( rs.m_enableBlend )
        EnableBlendingGL( rs.m_blendOP, rs.m_srcFactor, rs.m_dstFactor, forced );
    else
        DisableBlendingGL( forced );
    EnableDepthGL( rs.m_depthCompare, rs.m_depthWrite, forced );
    GL_CHECK_ERROR();
}


void Renderer::SetDebugWireframeShader()
{
    SetDebugShader( ShaderPass::GetWireframeDebugShader() );
}


void Renderer::SetDebugLightingShader()
{
    SetDebugShader( ShaderPass::GetLightingDebugShader() );
}


void Renderer::SetWindowUBO( Vec2 windowSize )
{
    if( m_globalUBOData.windowSize == windowSize )
        return;
    m_globalUBOData.windowSize = windowSize;
    m_globalUniformBuffer.Set( m_globalUBOData );
    m_globalUniformBuffer.BindToSlot( UBO::GLOBAL_BINDING );
    GL_CHECK_ERROR();
}


void Renderer::DrawLine( const Vec3& start, const Vec3& end, const Rgba& startColor,
                         const Rgba& endColor, float lineThickness )
{
    VertexPCU verts[2];
    verts[0].m_position = start;
    verts[0].m_color = startColor;
    verts[1].m_position = end;
    verts[1].m_color = endColor;

    glLineWidth( lineThickness );
    DrawMeshImmediate( verts, 2, nullptr, 0, DrawPrimitive::LINES );
}

void Renderer::DrawLine( const Vec3 & start, const Vec3 & end, const Rgba & color )
{
    DrawLine( start, end, color, color, 1.f );
}

void Renderer::DrawLines( const Vec3* vertPtrs, int numOfVerts, const Rgba& color )
{
    std::vector<VertexPCU> verts;
    verts.reserve( numOfVerts );
    for( int i = 0; i < numOfVerts; ++i )
    {
        verts.emplace_back( vertPtrs[i], color );
    }

    glLineWidth( 1.f );
    DrawMeshImmediate( verts.data(), numOfVerts, nullptr, 0,
                       DrawPrimitive::LINE_STRIP );
}

void Renderer::DrawLineLoop( const Vec3* vertPtrs, int numOfVerts, const Rgba& color )
{
    std::vector<VertexPCU> verts;
    verts.reserve( numOfVerts );
    for( int i = 0; i < numOfVerts; ++i )
    {
        verts.emplace_back( vertPtrs[i], color );
    }

    glLineWidth( 1.f );
    DrawMeshImmediate( verts.data(), numOfVerts, nullptr, 0, DrawPrimitive::LINE_LOOP );
}

// void Renderer::DrawDottedCircle( const Rgba& color, int sides )
// {
//
//     std::vector<VertexPCU> verts;
//     verts.reserve( sides );
//     float deltaAngleDegrees = 360.f / (float) sides;
//     for( int i = 0; i < sides; ++i )
//     {
//         verts.emplace_back( Vec2::MakeDirectionAtDegrees( deltaAngleDegrees * i ), color );
//     }
//
//     glLineWidth( 1.f );
//     DrawMeshImmediate( verts.data(), sides, nullptr, 0, DrawPrimitive::LINES );
// }

void Renderer::DrawRect( const AABB2& bounds, const Rgba& color )
{
    Vec3 verts[4];
    verts[0] = (Vec3) bounds.mins;
    verts[1] = (Vec3) bounds.GetMaxXMinY();
    verts[2] = (Vec3) bounds.maxs;
    verts[3] = (Vec3) bounds.GetMinXMaxY();
    DrawLineLoop( verts, 4, color );
}

void Renderer::DrawPoints( const std::vector<Vec3>& points, float pointSize /*= 1.f*/,
                           const Rgba& color /*= Rgba::WHITE */ )
{
    glPointSize( pointSize );
    std::vector<VertexPCU> verts;
    verts.reserve( points.size() );

    for( unsigned int pointIdx = 0; pointIdx < points.size(); ++pointIdx )
        verts.emplace_back( points[pointIdx], color );

    DrawMeshImmediate( verts.data(), (int) verts.size(), nullptr, 0,
                       DrawPrimitive::POINTS );
}


void Renderer::DrawCube( const Vec3& centerPos,
                         const Vec3& sideLengths,
                         const Rgba& tint,
                         const AABB2& uvTop,
                         const AABB2& uvSide,
                         const AABB2& uvBottom )
{

    Mesh* tempMesh = MeshPrimitive::MakeCube(
        sideLengths,
        tint,
        centerPos,
        uvTop,
        uvSide,
        uvBottom ).MakeMesh();

    DrawMeshWithTexture( tempMesh );
    delete tempMesh;
}


void Renderer::SetCurrentRenderState( const RenderState& state )
{
    //GetImmediateShader() = m_defaultShader;
    GetCurrentRenderState() = state;
}

void Renderer::DrawAABB( const AABB2& bounds, const Rgba& color )
{
    DrawTexturedAABB( Texture::GetWhiteTexture(), bounds, AABB2::ZEROS_ONES, color );
}

void Renderer::DrawTexturedAABB( const Texture* texture,
                                 const AABB2& bounds,
                                 const AABB2& uvCoords,
                                 const Rgba& tint )
{
    Mesh* tempMesh = MeshPrimitive::MakeQuad( bounds, tint, uvCoords ).MakeMesh();

    Renderable renderable{};
    renderable.SetMesh( tempMesh );
    renderable.GetMaterial( 0 )->SetDiffuse( texture );
    renderable.GetMaterial( 0 )->SetShaderPass( 0, ShaderPass::GetDefaultUIShader() );

    DrawRenderablePass( &renderable );

    delete tempMesh;
}


void Renderer::DrawText2D( const String& text,
                           const Vec2& position,
                           float height,
                           const BitmapFont* font,
                           const Rgba& color /*= Rgba::WHITE*/,
                           Alignment alignment /*= Alignment::CENTER_CENTER*/,
                           float aspectScale /*= 1.f */ )
{
    if( font == nullptr )
        font = m_defaultFont;

    // Set starting position to bottom left corner
    float aspectRatio = font->GetGlyphAspect( 0 ) * aspectScale;
    float textWidth = font->GetTextWidth( text, height, aspectRatio );
    Vec2 offset = AlignmentOffsetFromReference( alignment, Alignment::BOTTOM_LEFT );
    offset = Vec2( offset.x * textWidth, offset.y * height );
    Vec2 startPos = position + offset;

    float glyphWidth = height * aspectRatio;
    AABB2 glyphBounds = AABB2( 0, 0, glyphWidth, height );
    glyphBounds.Translate( startPos );

    const Texture* texture = font->GetTexture();
    for( unsigned int glyphIdx = 0; glyphIdx < text.length(); ++glyphIdx )
    {
        int glyphID = text[glyphIdx];
        AABB2 glyphUVs = font->GetGlyphUVs( glyphID );
        DrawTexturedAABB( texture, glyphBounds, glyphUVs, color );
        glyphBounds.Translate( glyphWidth, 0 );
    }
}

void Renderer::DrawTextInBox2D(
    const String& text,
    const AABB2& box,
    float fontHeight,
    const BitmapFont* font,
    TextDrawMode textDrawMode,
    const Vec2& alignment,
    const Rgba& color,
    float aspectScale )
{
    if( font == nullptr )
        font = m_defaultFont;

    Strings linesOfText;
    StringUtils::ParseToTokens( text, linesOfText, "\n", false );

    float boxWidth = box.GetWidth();
    float boxHeight = box.GetHeight();

    if( textDrawMode == TextDrawMode::WORD_WRAP )
    {
        Strings allWrappedLines;
        for( unsigned int lineIdx = 0; lineIdx < linesOfText.size(); ++lineIdx )
        {
            const String& line = linesOfText[lineIdx];
            Strings wrappedLines;
            font->WrapLineOfText( line, fontHeight, boxWidth, wrappedLines, aspectScale );
            allWrappedLines.insert( allWrappedLines.end(), wrappedLines.begin(), wrappedLines.end() );
        }
        linesOfText = allWrappedLines;
    }
    float textBlockHeight = (float) linesOfText.size() * fontHeight;

    if( textDrawMode == TextDrawMode::SHRINK_FIT || textDrawMode == TextDrawMode::WORD_WRAP )
    {
        float maxWidth = 0.f;
        for( auto& line : linesOfText )
        {
            float width = font->GetTextWidth( line, fontHeight, aspectScale );
            if( width > maxWidth )
                maxWidth = width;
        }
        float shrinkWidthRatio = boxWidth / maxWidth;
        float shrinkHeightRatio = boxHeight / textBlockHeight;
        float shrinkRatio = Minf( shrinkWidthRatio, shrinkHeightRatio );
        shrinkRatio = Clampf01( shrinkRatio );
        fontHeight *= shrinkRatio;
    }

    textBlockHeight = (float) linesOfText.size() * fontHeight; // recalculate, since font height has changed
    Vec2 pos;
    float totalYPadding = boxHeight - textBlockHeight;
    float paddingOnTop = ( 1.f - alignment.y ) * totalYPadding;
    pos.y = box.maxs.y - paddingOnTop;
    for( unsigned int lineIdx = 0; lineIdx < linesOfText.size(); ++lineIdx )
    {
        String& line = linesOfText[lineIdx];
        float totalXPaddingForLine = boxWidth - font->GetTextWidth( line, fontHeight, aspectScale );
        float paddingOnLeft = alignment.x * totalXPaddingForLine;
        pos.x = box.mins.x + paddingOnLeft;
        DrawText2D( line, pos, fontHeight, font, color, Alignment::TOP_LEFT, aspectScale );
        pos.y -= fontHeight;
    }
}

void Renderer::DrawText2DFlashWave(
    const String& text,
    const Vec2& position,
    float height,
    const BitmapFont* font,
    float time,
    const Rgba& colorA /*= Rgba::WHITE*/,
    const Rgba& colorB /*= Rgba::BLACK*/,
    float interval /*= 1.f*/,
    float flashDuration /*= 0.1f*/,
    float nextGlyphOffsetSeconds /*= 0.01f*/,
    const Rgba& colorFlash /*= Rgba::WHITE*/,
    float bumpUpRatio /*= 0.1f*/,
    Alignment alignment /*= Alignment::CENTER_CENTER*/,
    float aspectScale /*= 1.f */ )

{
    if( font == nullptr )
        font = m_defaultFont;

    float aspectRatio = font->GetGlyphAspect( 0 ) * aspectScale;
    // Set starting position to bottom left corner
    float textWidth = font->GetTextWidth( text, height, aspectRatio );
    Vec2 offset = AlignmentOffsetFromReference( alignment, Alignment::BOTTOM_LEFT );
    offset = Vec2( offset.x * textWidth, offset.y * height );
    Vec2 startPos = position + offset;

    float glyphWidth = height * aspectRatio;
    AABB2 glyphBounds = AABB2( 0, 0, glyphWidth, height );
    glyphBounds.Translate( startPos );

    float maxBumpUpHeight = bumpUpRatio * height;

    const Texture* texture = font->GetTexture();
    for( unsigned int glyphIdx = 0; glyphIdx < text.length(); ++glyphIdx )
    {
        float localTime = time - glyphIdx * nextGlyphOffsetSeconds;
        float periodicBellWave = Easing::Bell3( Wave::PeriodicSawTooth( localTime, interval, flashDuration ) );
        float bumpUpOffset = periodicBellWave * maxBumpUpHeight;
        AABB2 glyphBoundsBumpedUp = glyphBounds;
        glyphBoundsBumpedUp.Translate( 0, bumpUpOffset );

        int cycle = FloorToInt( ( localTime - flashDuration / 2.f ) / interval );
        Random::SetSeed( cycle );
        Rgba color = Random::ColorInRange( colorA, colorB );
        color = Lerp( color, colorFlash, periodicBellWave );

        int glyphID = text[glyphIdx];
        AABB2 glyphUVs = font->GetGlyphUVs( glyphID );
        DrawTexturedAABB( texture, glyphBoundsBumpedUp, glyphUVs, color );
        glyphBounds.Translate( glyphWidth, 0 );
    }
}

Texture* Renderer::CreateOrGetTexture( const String& texturePath )
{
    if( m_loadedTextures.find( texturePath ) == m_loadedTextures.end() )
    {
        m_loadedTextures[texturePath] = new Texture( texturePath );  // Loading error handled in Texture constructor
        if( m_loadedTextures[texturePath]->Valid() )
        {
            return m_loadedTextures[texturePath];
        }
        else
        {
            delete m_loadedTextures[texturePath];
            m_loadedTextures.erase( texturePath );
            return nullptr;
        }
    }
    else
    {
        return m_loadedTextures[texturePath];
    }
}

Texture* Renderer::CreateOrGetTexture( Image* image )
{
    return new Texture( image );
}

Texture* Renderer::CreateRenderTarget( uint width, uint height, TextureFormat format /*= TextureFormat::RGBA8 */ )
{
    Texture *tex = new Texture();
    tex->CreateRenderTarget( width, height, format );
    return tex;
}

Texture* Renderer::CreateDepthStencilTarget( uint width, uint height )
{
    return CreateRenderTarget( width, height, TextureFormat::D24S8 );
}

void Renderer::TakeScreenshot()
{
    Image* image = m_defaultColorTarget->MakeImageFromGPU();
    image->FlipYCoords();
    image->SaveToDisk( "Screenshots/screenshot.png" );
    TimeUtils::SysTime sysTime = TimeUtils::GetDateTime();
    String path = Stringf( "Screenshots/screenshot_%i.%i.%i_%i.%i.%i.png",
                           sysTime.year, sysTime.month, sysTime.day,
                           sysTime.hour, sysTime.minute, sysTime.second );
    image->SaveToDisk( path );
    delete image;

}

void Renderer::SetCullModeGL( CullMode cull, bool forced /*= false */ )
{
    if( ( m_currentRenderState.m_cullMode != cull ) || forced )
    {
        m_currentRenderState.m_cullMode = cull;
        if( CullMode::NONE == cull )
            glDisable( GL_CULL_FACE );
        else
        {
            glEnable( GL_CULL_FACE );
            glCullFace( ToGLEnum( cull ) );
        }
    }
}

void Renderer::SetFillModeGL( FillMode fill, bool forced /*= false */ )
{
    if( ( m_currentRenderState.m_fillMode != fill ) || forced )
    {
        m_currentRenderState.m_fillMode = fill;
        glPolygonMode( GL_FRONT_AND_BACK, ToGLEnum( fill ) );
    }
}

void Renderer::SetWindOrderGL( WindOrder windOrder, bool forced /*= false */ )
{
    if( ( m_currentRenderState.m_windOrder != windOrder ) || forced )
    {
        m_currentRenderState.m_windOrder = windOrder;
        glFrontFace( ToGLEnum( windOrder ) );
    }
}

// void Renderer::EnableBlending( BlendMode blendMode )
// {
//     BlendOP op;
//     BlendFactor src;
//     BlendFactor dst;
//     BreakOutBlendMode( blendMode, op, src, dst );
//     EnableBlending( op, src, dst );
// }
//
// void Renderer::EnableBlending( BlendOP op, BlendFactor src, BlendFactor dst )
// {
//     GetCurrentRenderState().m_enableBlend = true;
//     GetCurrentRenderState().m_blendOP = op;
//     GetCurrentRenderState().m_srcFactor = src;
//     GetCurrentRenderState().m_dstFactor = dst;
// }
//
// void Renderer::DisableBlending()
// {
//     GetCurrentRenderState().m_enableBlend = false;
// }
//
// void Renderer::EnableDepth( DepthCompareMode compare /*= DepthCompareMode::LESS*/,
//                             bool shouldWrite /*= true */ )
// {
//     GetCurrentRenderState().m_depthCompare = compare;
//     GetCurrentRenderState().m_depthWrite = shouldWrite;
// }

// void Renderer::DisableDepth()
// {
//     EnableDepth( DepthCompareMode::ALWAYS, false );
// }

void Renderer::EnableBlendingGL( BlendOP op, BlendFactor src, BlendFactor dst, bool forced /*= false */ )
{
    if( ( !m_currentRenderState.m_enableBlend ) || forced )
    {
        m_currentRenderState.m_enableBlend = true;
        glEnable( GL_BLEND );
    }
    if( ( m_currentRenderState.m_blendOP != op ) || forced )
    {
        m_currentRenderState.m_blendOP = op;
        glBlendEquation( ToGLEnum( op ) );
    }

    if( ( m_currentRenderState.m_srcFactor != src )
        || ( m_currentRenderState.m_dstFactor != dst )
        || forced )
    {
        m_currentRenderState.m_srcFactor = src;
        m_currentRenderState.m_dstFactor = dst;
        glBlendFunc( ToGLEnum( src ), ToGLEnum( dst ) );
    }
}

void Renderer::DisableBlendingGL( bool forced /*= false */ )
{
    if( ( m_currentRenderState.m_enableBlend ) || forced )
    {
        m_currentRenderState.m_enableBlend = false;
        glDisable( GL_BLEND );
    }
}

void Renderer::EnableDepthGL( DepthCompareMode compare /*= DepthCompareMode::LESS*/,
                              bool shouldWrite /*= true*/, bool forced /*= false */ )
{
    if( ( m_currentRenderState.m_depthCompare != compare ) || forced )
    {
        m_currentRenderState.m_depthCompare = compare;
        glDepthFunc( ToGLEnum( compare ) );
    }
    if( ( m_currentRenderState.m_depthWrite != shouldWrite ) || forced )
    {
        m_currentRenderState.m_depthWrite = shouldWrite;
        glDepthMask( shouldWrite ? GL_TRUE : GL_FALSE );
    }
}

void Renderer::DisableDepthGL( bool forced /*= false */ )
{
    EnableDepthGL( DepthCompareMode::ALWAYS, false, forced );
}

void Renderer::ClearDepth( float depth /*= 1.0f */ )
{
    glDepthMask( GL_TRUE );
    glClearDepthf( depth );
    glClear( GL_DEPTH_BUFFER_BIT );
}

Texture* Renderer::GetTexture( const String& texturePath )
{
    if( m_loadedTextures.find( texturePath ) == m_loadedTextures.end() )
    {
        LOG_MISSING_ASSET( texturePath );
        return nullptr;
    }
    return m_loadedTextures[texturePath];
}

bool Renderer::CopyFrameBuffer( FrameBuffer *dst, FrameBuffer *src )
{
    GL_CHECK_ERROR();
    // we need at least the src.
    if( src == nullptr )
    {
        return false;
    }

    // Get the handles - NULL refers to the "default" or back buffer FBO
    GLuint src_fbo = src->GetHandle();
    GLuint dst_fbo = NULL;
    if( dst != nullptr )
    {
        dst_fbo = dst->GetHandle();
    }

    // can't copy onto ourselves
    if( dst_fbo == src_fbo )
    {
        return false;
    }

    // the GL_READ_FRAMEBUFFER is where we copy from
    glBindFramebuffer( GL_READ_FRAMEBUFFER, src_fbo );

    // what are we copying to?
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, dst_fbo );

    // blit it over - get the size
    // (we'll assume dst matches for now - but to be safe,
    // you should get dst_width and dst_height using either
    // dst or the window depending if dst was nullptr or not
    uint width = src->GetDimensions().x;
    uint height = src->GetDimensions().y;

    // Copy it over
    glBlitFramebuffer( 0, 0, // src start pixel
                       width, height,        // src size
                       0, 0,                 // dst start pixel
                       width, height,        // dst size
                       GL_COLOR_BUFFER_BIT,  // what are we copying (just colour)
                       GL_NEAREST );         // resize filtering rule (in case src/dst don't match)

                                             // Make sure it succeeded
    GL_CHECK_ERROR();

    // cleanup after ourselves
    glBindFramebuffer( GL_READ_FRAMEBUFFER, NULL );
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, NULL );

    return GLSucceeded();
}

void Renderer::SetDefaultFont( const String& bitmapFontPath )
{
    m_defaultFont = CreateOrGetBitmapFont( bitmapFontPath );
}

BitmapFont* Renderer::CreateOrGetBitmapFont( const String& bitmapFontPath )
{
    if( m_loadedFonts.find( bitmapFontPath ) == m_loadedFonts.end() )
    {
        Texture* texture = CreateOrGetTexture( bitmapFontPath );  // Loading error handled in Texture constructor
        m_loadedFonts[bitmapFontPath] = new BitmapFont( texture );
        return m_loadedFonts[bitmapFontPath];
    }
    else
    {
        return m_loadedFonts[bitmapFontPath];
    }
}

BitmapFont* Renderer::GetBitmapFont( const String& bitmapFontPath )
{
    if( m_loadedFonts.find( bitmapFontPath ) == m_loadedFonts.end() )
    {
        LOG_MISSING_ASSET( bitmapFontPath );
        return nullptr;
    }
    return m_loadedFonts[bitmapFontPath];
}

SpriteSheet* Renderer::CreateOrGetSpriteSheet( const String& spriteSheetPath, const IVec2& layout )
{
    if( m_loadedSpriteSheets.find( spriteSheetPath ) == m_loadedSpriteSheets.end() )
    {
        Texture* texture = CreateOrGetTexture( spriteSheetPath );
        if( !texture )
            return nullptr;
        m_loadedSpriteSheets[spriteSheetPath] = new SpriteSheet( texture, layout );
        return m_loadedSpriteSheets[spriteSheetPath];
    }
    else
    {
        return m_loadedSpriteSheets[spriteSheetPath];
    }
}

SpriteSheet* Renderer::GetSpriteSheet( const String& spriteSheetPath )
{
    if( m_loadedSpriteSheets.find( spriteSheetPath ) == m_loadedSpriteSheets.end() )
    {
        LOG_MISSING_ASSET( spriteSheetPath );
        return nullptr;
    }
    return m_loadedSpriteSheets[spriteSheetPath];
}

// void Renderer::UseDefaultTexture()
// {
//     SetImmediateTexture( m_whiteTexture );
// }

// void Renderer::UseDefaultShader()
// {
//     GetImmediateShader() = m_defaultShader;
//     GetImmediateShaderProgram() = m_defaultShaderProgram;
//     GetCurrentRenderState() = RenderState{};
// }

void Renderer::SetAmbient( const Vec4& color )
{
    m_lightUBOData.ambient = color;
    BindLightUBO();
}

void Renderer::SetLight( uint idx, const Vec4& color, const Vec3& position,
                         float sourceRadius,
                         float isPointLight /*= 1*/,
                         const Vec3& direction,
                         float coneInnerDot /*= -2 */, float coneOuterDot /*= -2 */ )
{
    if( idx > MAX_LIGHTS - 1 )
    {
        LOG_WARNING( "Trying to use more lights than supported!" );
        return;
    }

    m_lightUBOData.lights[idx].color = color;
    m_lightUBOData.lights[idx].position = position;
    m_lightUBOData.lights[idx].isPointLight = isPointLight;
    m_lightUBOData.lights[idx].direction = direction;
    m_lightUBOData.lights[idx].coneInnerDot = coneInnerDot;
    m_lightUBOData.lights[idx].coneOuterDot = coneOuterDot;
    m_lightUBOData.lights[idx].sourceRadius = sourceRadius;


}

void Renderer::SetLight( uint idx, Light* light )
{
    Vec4 color = Vec4( light->m_color.ToVec3(), light->m_intensity );
    SetLight( idx, color, light->GetTransform().GetWorldPosition(),
              light->m_sourceRadius,
              light->m_isPointLight,
              light->GetTransform().GetForward(),
              light->m_coneInnerDot,
              light->m_coneOuterDot );
}

void Renderer::SetPointLight( uint idx, const Vec4& color, const Vec3& position,
                              float sourceRadius /*= 1 */ )
{
    SetLight( idx, color, position, sourceRadius );
}

void Renderer::SetDirectionalLight( uint idx, const Vec4& color, const Vec3& position,
                                    const Vec3& direction /*= Vec3( 1, 0, 0 )*/ )
{
    float sourceRadius = 99999.f;
    SetLight( idx, color, position, sourceRadius, 0, direction );
}

void Renderer::SetSpotLight( uint idx, const Vec4& color, const Vec3& position,
                             float sourceRadius /*= 1*/,
                             const Vec3& direction /*= Vec3( 1, 0, 0 )*/,
                             float coneInnerDot /*= -2*/, float coneOuterDot /*= -2 */ )
{
    SetLight( idx, color, position, sourceRadius, 1, direction,
              coneInnerDot, coneOuterDot );
}

void Renderer::DisableAllLights()
{
    for( int lightIdx = 0; lightIdx < MAX_LIGHTS; ++lightIdx )
    {
        SetLight( lightIdx, Vec4::ZEROS, Vec3::ZEROS );
    }
}

void Renderer::DeleteTexture( Texture*& texture )
{
    if( !texture )
    {
        LOG_NULL_POINTER( "Texture" );
        return;
    }
    delete texture;
    texture = nullptr;
}

RenderState& Renderer::GetCurrentRenderState()
{
    return m_currentRenderState;
}

uint Renderer::BindShader( ShaderPass* shader )
{
    ShaderProgram* shaderProgram = nullptr;
    if( shader )
    {
        BindRenderState( shader->m_state );
        shaderProgram = shader->m_program;
    }
    else
    {
        shaderProgram = ShaderProgram::GetDefaultProgram();

        BindRenderState( RenderState::GetDefault() );
    }

    if( !shaderProgram )
        shaderProgram = ShaderProgram::GetDefaultProgram();

    GLuint programHandle = shaderProgram->GetHandle();
    if( programHandle == NULL )
    {
        shaderProgram = ShaderProgram::GetInvalidProgram();
        BindRenderState( ShaderPass::GetInvalidShader()->m_state );
    }
    shaderProgram->Bind();
    GL_CHECK_ERROR();
    return programHandle;
}

void Renderer::UseCamera( Camera* camera )
{
    if( camera == nullptr )
    {
        camera = m_defaultCamera;
    }

    // make sure the framebuffer is finished being setup;
    camera->Finalize();

    m_currentCamera = camera;

    m_cameraUBOData.projection = m_currentCamera->GetProjMatrix();
    m_cameraUBOData.view = m_currentCamera->GetViewMatrix();
    m_cameraUBOData.vp = m_currentCamera->GetVPMatrix();
    m_cameraUBOData.cameraPosition = m_currentCamera->GetTransform().GetWorldPosition();

    m_cameraUniformBuffer.Set( m_cameraUBOData );
    m_cameraUniformBuffer.BindToSlot( UBO::CAMERA_BINDING );

    glBindFramebuffer( GL_FRAMEBUFFER, m_currentCamera->GetFrameBufferHandle() );
}

void Renderer::BindInstanceUBO( const Mat4& model, const Material* material )
{
    GL_CHECK_ERROR();
    m_instanceUBOData.model = model;
    m_instanceUBOData.mvp = m_cameraUBOData.vp * model;

    m_instanceUBOData.specularAmount = material->m_specularAmount;
    m_instanceUBOData.specularPower = material->m_specularPower;
    m_instanceUBOData.tint = material->m_tint.ToVec4();

    m_instanceUniformBuffer.Set( m_instanceUBOData );
    m_instanceUniformBuffer.BindToSlot( UBO::INSTANCE_BINDING );
    GL_CHECK_ERROR();
}


void Renderer::BindLightUBO()
{
    m_lightUniformBuffer.Set( m_lightUBOData );
    m_lightUniformBuffer.BindToSlot( UBO::LIGHT_BINDING );
    GL_CHECK_ERROR();
}

void Renderer::UseDefaultCamera()
{
    UseCamera( m_defaultCamera );
}

void Renderer::UseUICamera()
{
    UseCamera( m_UICamera );
}

void Renderer::BindSampler( uint textureUnitIdx, Sampler* sampler )
{
    glBindSampler( textureUnitIdx, sampler->GetHandle() );
}

void Renderer::BindTexture( uint textureUnitIdx, const Texture* texture )
{
    if( !texture )
        texture = Texture::GetWhiteTexture();
    glActiveTexture( GL_TEXTURE0 + textureUnitIdx );
    glBindTexture( ToGLEnum( texture->m_target ), texture->GetHandle() );
    GL_CHECK_ERROR();
}

void Renderer::ApplyEffect( Material* material )
{
    UNUSED( material );

    //     UseCamera( m_effectCamera );
    //
    //     SetImmediateShaderProgram( program );
    //
    //     //BindTexture( 0, m_whiteTexture );
    //
    //     //BindSampler( 0, m_pointSampler );
    //
    //     BindTexture( 1, m_defaultDepthTarget );
    //
    //     BindSampler( 1, m_pointSampler );
    //
    //     DrawTexturedAABB( m_defaultColorTarget );
    //
    //     Texture::SwapHandle( m_defaultColorTarget, m_effectColorTarget );

}

void Renderer::SetClocks( Clock* gameClock, Clock* appClock )
{
    m_gameClock = gameClock;
    m_appClock = appClock;
}

void Renderer::UpdateAndBindTimeUBO()
{
    if( nullptr != m_gameClock )
    {
        m_timeUBOData.gameDeltaSeconds = m_gameClock->GetDeltaSecondsF();
        m_timeUBOData.gameSeconds = m_gameClock->GetTimeSinceStartupF();
    }
    if( nullptr != m_appClock )
    {
        m_timeUBOData.appDeltaSeconds = m_appClock->GetDeltaSecondsF();
        m_timeUBOData.appSeconds = m_appClock->GetTimeSinceStartupF();
    }

    // Update and bind the time buffer
    m_timeBuffer.Set( m_timeUBOData );
    m_timeBuffer.BindToSlot( UBO::TIME_BINDING );
    GL_CHECK_ERROR();
}

Vec2 AlignmentOffsetFromCenter( Alignment alignment )
{
    switch( alignment )
    {
    case Alignment::TOP_LEFT:
        return Vec2( -0.5f, 0.5f );

    case Alignment::TOP_CENTER:
        return Vec2( 0.f, 0.5f );

    case Alignment::TOP_RIGHT:
        return Vec2( 0.5f, 0.5f );

    case Alignment::CENTER_LEFT:
        return Vec2( -0.5f, 0.f );

    case Alignment::CENTER_CENTER:
        return Vec2( 0.f, 0.f );

    case Alignment::CENTER_RIGHT:
        return Vec2( 0.5f, 0.f );

    case Alignment::BOTTOM_LEFT:
        return Vec2( -0.5f, -0.5f );

    case Alignment::BOTTOM_CENTER:
        return Vec2( 0.f, -0.5f );

    case Alignment::BOTTOM_RIGHT:
        return Vec2( 0.5f, -0.5f );

    default:
        return Vec2( 0.f, 0.f );
    }
}

Vec2 AlignmentOffsetFromReference( Alignment reference, Alignment offsetAlignment )
{
    return AlignmentOffsetFromCenter( offsetAlignment ) - AlignmentOffsetFromCenter( reference );
}
