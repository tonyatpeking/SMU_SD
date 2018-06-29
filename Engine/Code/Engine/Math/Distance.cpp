#include "Engine/Math/Distance.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Plane.hpp"

namespace Distance
{

float PointPlane( const Vec3& point, const Plane& plane )
{
    return plane.GetDistance( point );
}

float PointSphere( const Vec3& point, const Vec3& center, float radius )
{
    return Distance::SphereSphere( center, radius, point, 0.f );
}

float SphereSphere( const Vec3& center1, float radius1, const Vec3& center2, float radius2 )
{
    float distTotal = Vec3::GetDistance( center1, center2 );
    float radiusSum = ( radius1 + radius2 );
    return distTotal - radiusSum;
}

}
