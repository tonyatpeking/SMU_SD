#include "Engine/Math/Frustum.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"

Frustum Frustum::FromMatrixAABB3(
    const Mat4& mat4, const AABB3& ndc /*= AABB3( Vec3::ZEROS, 2, 2, 2 ) */ )
{
    Vec3 corners[8];
    ndc.GetCorners( corners );

    Mat4 inv = mat4.Inverse();

    Frustum frustum;

    for (int i = 0; i < 8 ; ++i)
    {
        Vec4 ndcPos = Vec4( corners[i], 1 );
        Vec4 kindaWorldPos = inv * ndcPos;

        frustum.m_worldCorners[i] = Vec3(kindaWorldPos) / kindaWorldPos.w;
    }

    Vec3* worldCorners = frustum.m_worldCorners;

    // vertex order
    //                7-------6
    //               /|      /|
    //              3-|-----2 |
    //              | 4-----|-5
    //              |/   o  |/
    //              0-------1
    // plane normals should be sticking out
    frustum.m_right = Plane( worldCorners[1], worldCorners[5], worldCorners[6] );
    frustum.m_left = Plane( worldCorners[4], worldCorners[0], worldCorners[3] );
    frustum.m_top = Plane( worldCorners[3], worldCorners[2], worldCorners[6] );
    frustum.m_bottom = Plane( worldCorners[4], worldCorners[5], worldCorners[1] );
    frustum.m_forward = Plane( worldCorners[7], worldCorners[6], worldCorners[5] );
    frustum.m_backward = Plane( worldCorners[0], worldCorners[1], worldCorners[2] );

    return frustum;
}
