#pragma once
#include "Engine/Math/Plane.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Math/AABB3.hpp"

class Frustum
{
public:
    Frustum() {};
    ~Frustum() {};
    static Frustum FromMatrixAABB3(
        const Mat4& mat4,
        const AABB3& ndc = AABB3( Vec3::ZEROS, 2, 2, 2 ) );

    // all face planes, normals point outward
    const Plane& Right() const { return m_right; };
    const Plane& Left() const { return m_left; };
    const Plane& Top() const { return m_top; };
    const Plane& Bottom() const { return m_bottom; };
    const Plane& Forward() const { return m_forward; };
    const Plane& Backward() const { return m_backward; };

    const Vec3* GetCorners() const { return m_worldCorners; };

private:
    //disable VS warning
#pragma warning(disable : 4201)
    union
    {
        // planes are in world space
        Plane m_planes[6];
        struct
        {
            Plane m_right;
            Plane m_left;
            Plane m_top;
            Plane m_bottom;
            Plane m_forward;
            Plane m_backward;
        };
    };
#pragma warning(default : 4201)

    Vec3 m_worldCorners[8];
};
