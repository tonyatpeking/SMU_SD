#pragma once

class Vec3;
class Plane;

namespace Distance
{
// can be negative
float PointPlane( const Vec3& point, const Plane& plane );
float PointSphere( const Vec3& point, const Vec3& center, float radius );
float SphereSphere( const Vec3& center1, float radius1,
                    const Vec3& center2, float radius2 );

}