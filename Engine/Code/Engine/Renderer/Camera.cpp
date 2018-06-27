#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/FrameBuffer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"


Camera::Camera()
    : GameObject("Camera")
{

}

Camera::~Camera()
{
    delete m_frameBuffer;
}

void Camera::SetColorTarget( Texture *colorTarget )
{
    if( !m_frameBuffer )
        m_frameBuffer = new FrameBuffer();
    m_frameBuffer->SetColorTarget( colorTarget );
}

void Camera::SetDepthStencilTarget( Texture *depthTarget )
{
    if( !m_frameBuffer )
        m_frameBuffer = new FrameBuffer();
    m_frameBuffer->SetDepthStencilTarget( depthTarget );
}

Vec2 Camera::WorldToScreen( Vec3 worldPos )
{
    Vec4 clip = GetVPMatrix() * Vec4( worldPos, 1.f );
    Vec3 ndc = Vec3( clip ) / clip.w;
    // point is behind us, hacky solution so that point is not transformed to screen
    if( Dot( worldPos - m_transform.GetWorldPosition(), m_transform.GetForward()) < 0 )
    {
        return Vec2( -1000000, -1000000 );
    }
    Vec2 bufferDim = (Vec2) m_frameBuffer->GetDimensions();
    return RangeMap( Vec2( ndc ), Vec2::NEG_ONES, Vec2::ONES, Vec2( 0.f, bufferDim.y ),
                     Vec2( bufferDim.x, 0.f ) );
}

Vec2 Camera::WorldToScreenInvertY( Vec3 worldPos )
{
    Vec2 worldToScreen = WorldToScreen( worldPos );
    Vec2 bufferDim = (Vec2) m_frameBuffer->GetDimensions();
    worldToScreen.y = bufferDim.y - worldToScreen.y;
    return worldToScreen;
}

Vec3 Camera::ScreenToWorld( Vec2 pixel, float depthFromCamera )
{
    Vec2 bufferDim = (Vec2) m_frameBuffer->GetDimensions();
    Vec2 ndcXY = RangeMap( pixel, Vec2( 0.f, bufferDim.y ), Vec2( bufferDim.x, 0.f ), Vec2::NEG_ONES, Vec2::ONES );

    // Figure out distance from camera - what is it's z.
    Vec4 clip = m_projMat * Vec4( 0.f, 0.f, depthFromCamera, 1.f );
    Vec3 ndc = Vec3( clip ) / clip.w;
    ndc = Vec3( ndcXY, ndc.z );

    clip = Vec4( ndc, 1.f );

    // convert from ndc to world;
    Mat4 inverseVP = GetVPMatrix();
    bool success = inverseVP.Invert();
    if( !success )
        LOG_WARNING( "ScreenToWorld failed because matrix inverse failed" );
    Vec4 kindaWorld = inverseVP * clip;
    Vec3 world = Vec3( kindaWorld ) / kindaWorld.w;

    return world;
}

Ray3 Camera::ScreenToPickRay( Vec2 pixel )
{
    Vec3 startPoint = ScreenToWorld( pixel, 0.5f );
    Vec3 endPoint = ScreenToWorld( pixel, 1.f );

    return Ray3( startPoint, endPoint - startPoint );
}

void Camera::SetProjection( Mat4 proj )
{
    m_projMat = proj;
}


void Camera::SetProjectionOrtho( float height, float near, float far )
{
    float width = height * m_frameBuffer->GetAspect();
    m_projMat = Mat4::MakeOrtho( width, height, near, far );

}

void Camera::SetProjectionOrtho( AABB2 bounds, float near, float far )
{
    m_projMat = Mat4::MakeOrtho( bounds.mins, bounds.maxs, near, far );
}

void Camera::SetProjection( float fovVertDeg, float near, float far )
{
    float aspect = m_frameBuffer->GetAspect();
    m_projMat = Mat4::MakeProjection( fovVertDeg, aspect, near, far );
}

void Camera::Finalize()
{
    m_frameBuffer->Finalize();
}

uint Camera::GetFrameBufferHandle()
{
    return m_frameBuffer->GetHandle();
}

Mat4 Camera::GetVPMatrix()
{
    return m_projMat * GetViewMatrix();
}
