#include "Engine/Math/Plane.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"

Plane::Plane( const Vec3& a, const Vec3& b, const Vec3& c )
{
    //    c
    //   / \
    //  /___\
    // a     b
    Vec3 edge0 = b - a;
    Vec3 edge1 = c - a;
    normal = Cross( edge1, edge0 );
    float length = normal.NormalizeAndGetLength();
    if( AlmostEqual( length, 0 ) )
    {
        normal = Vec3::UP;
        LOG_WARNING( "Trying to construct plane with colinear points!" );
    }
    distance = Dot( normal, a );
}

void Plane::FlipNormal()
{
    normal = -normal;
    distance = -distance;
}

float Plane::GetDistance( const Vec3& pos ) const
{
    // project to normal, distance to origin
    float dist = Dot( pos, normal );
    return dist - distance;
}

bool Plane::OnPositiveSide( const Vec3& pos ) const
{
    return GetDistance( pos ) > 0.f;
}
