#include "Engine/Math/Intersection.hpp"
#include "Engine/Math/Segment3.hpp"
#include "Engine/Math/Plane.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/AABB3.hpp"


namespace Intersect
{



bool LinePlane( const Vec3& pointOnLine, const Vec3& lineDir,
                const Vec3& pointOnPlane, const Vec3& planeNormal,
                Vec3& out_intersectionPoint )
{
    //from https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection

    float divisor = Dot( lineDir, planeNormal );
    if( 0.f == divisor )
        return false;

    float d = Dot( pointOnPlane - pointOnLine, planeNormal ) / divisor;
    out_intersectionPoint = d * lineDir + pointOnLine;

    return true;
}

bool SegmentPlane( const Segment3& s, const Plane& p )
{
    float d0 = p.GetDistance( s.start );
    float d1 = p.GetDistance( s.end );

    return ( d0 * d1 ) <= 0.0f;
}

bool SphereSphere( const Vec3& center1, float radius1,
                   const Vec3& center2, float radius2 )
{
    float distSquared = Vec3::GetDistanceSquared( center1, center2 );
    float radiusSumSquared = ( radius1 + radius2 ) * ( radius1 + radius2 );
    return distSquared < radiusSumSquared;
}

bool PointSphere( const Vec3& point, const Vec3& center, float radius )
{
    return Intersect::SphereSphere( center, radius, point, 0.f );
}

bool IsPointInsideConvexPolyhedron( const Vec3& point,
                                    const vector<Plane>& polyhedron )
{
    return IsSphereInsideConvexPolyhedron( point, 0, polyhedron );
}

bool IsPointOutsideConvexPolyhedron( const Vec3& point,
                                     const vector<Plane>& polyhedron )
{
    return !IsPointInsideConvexPolyhedron( point, polyhedron );
}

bool IsSphereInsideConvexPolyhedron( const Vec3& center, float radius,
                                     const vector<Plane>& polyhedron )
{
    for( auto& plane : polyhedron )
    {
        if( !plane.IsSphereAllNegative( center, radius ) )
            return false;
    }
    return true;
}

bool IsSphereInsideAABB3( const Vec3& center, float radius, const AABB3& aabb3 )
{
    return IsSphereInsideHexahedron( center, radius, aabb3 );
}

bool IsSphereOutsideAABB3( const Vec3& center, float radius, const AABB3& aabb3 )
{
    return IsSphereOutsideHexahedron( center, radius, aabb3 );
}

bool IsSphereIntersectAABB3( const Vec3& center, float radius, const AABB3& aabb3 )
{
    return IsSphereIntersectHexahedron( center, radius, aabb3 );
}

bool IsSphereInsideOBB3( const Vec3& center, float radius, const OBB3& obb3 )
{
    return IsSphereInsideHexahedron( center, radius, obb3 );
}

bool IsSphereOutsideOBB3( const Vec3& center, float radius, const OBB3& obb3 )
{
    return IsSphereOutsideHexahedron( center, radius, obb3 );
}

bool IsSphereIntersectOBB3( const Vec3& center, float radius, const OBB3& obb3 )
{
    return IsSphereIntersectHexahedron( center, radius, obb3 );
}




}


