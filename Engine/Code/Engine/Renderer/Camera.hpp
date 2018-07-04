#pragma once
#include "Engine/Math/Mat4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Ray3.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Core/Rgba.hpp"

class Texture;
class Vec3;
class Mat4;
class Material;
class FrameBuffer;
class RenderSceneGraph;


class Camera : public GameObject
{
public:
    Camera();
    virtual ~Camera();

    void SetColorTarget( Texture *colorTarget );
    void SetDepthStencilTarget( Texture *depthTarget );

    // model setters
    //void LookAt( Vec3 position, Vec3 target, Vec3 up = Vec3::UP );

    // returns a pixel coordinate this world coordinate would go to.
    //(0, 0) being the top-left pixel
    Vec2 WorldToScreen( Vec3 worldPos );
    Vec2 WorldToScreenInvertY( Vec3 worldPos );
    Vec3 ScreenToWorld( Vec2 pixel, float depthFromCamera );
    Ray3 ScreenToPickRay( Vec2 pixel );


    // projection settings
    void SetProjection( Mat4 proj );
    void SetProjectionOrtho( float height, float near, float far );
    void SetProjectionOrtho( AABB2 bounds, float near, float far );
    void SetProjection( float fovVertDeg, float near, float far );

    void Finalize();
    uint GetFrameBufferHandle();

    Mat4 GetVPMatrix();
    const Mat4& GetCamToWorldMatrix() const { return m_transform.GetLocalToWorld(); };
    const Mat4& GetViewMatrix() { return m_transform.GetWorldToLocal(); };
    const Mat4& GetProjMatrix() const { return m_projMat; };

    FrameBuffer* GetFrameBuffer() { return m_frameBuffer; };

    // Debug
    void SetFrustumVisible( bool visible, const Rgba& color = Rgba::RED_MAGENTA );


    // default all to identity
    //Mat4 m_camToWorld;  // where is the camera?
    //Mat4 m_viewMat;    // inverse of camera (used for shader)
    Mat4 m_projMat;    // projection

    FrameBuffer* m_frameBuffer = nullptr;
    std::vector<Material*> m_effects;


private:

};