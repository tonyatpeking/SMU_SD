#include "Engine/Math/Intersection.hpp"
#include "Engine/Math/Segment3.hpp"
#include "Engine/Math/Plane.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/AABB3.hpp"

namespace
{

// see http://theorangeduck.com/page/correct-box-sphere-intersection

template <typename BoundingBox>
bool IsSphereInsideBox( const Vec3& center, float radius, const BoundingBox& bbox )
{
    if( !bbox.Left().IsSphereAllNegative( center, radius ) )
        return false;
    if( !bbox.Right().IsSphereAllNegative( center, radius ) )
        return false;
    if( !bbox.Top().IsSphereAllNegative( center, radius ) )
        return false;
    if( !bbox.Bottom().IsSphereAllNegative( center, radius ) )
        return false;
    if( !bbox.Forward().IsSphereAllNegative( center, radius ) )
        return false;
    if( !bbox.Backward().IsSphereAllNegative( center, radius ) )
        return false;

    return true;
}

template <typename BoundingBox>
bool IsSphereIntersectBox( const Vec3& center, float radius, const BoundingBox& bbox )
{
    // notOut = not outside includes inside or intersecting
    bool notOutLeft       = !bbox.Left().IsSphereAllPositive( center, radius );
    bool notOutRight      = !bbox.Right().IsSphereAllPositive( center, radius );
    bool notOutTop        = !bbox.Top().IsSphereAllPositive( center, radius );
    bool notOutBottom     = !bbox.Bottom().IsSphereAllPositive( center, radius );
    bool notOutForward    = !bbox.Forward().IsSphereAllPositive( center, radius );
    bool notOutBackward   = !bbox.Backward().IsSphereAllPositive( center, radius );

    if( bbox.Left().IsSphereIntersecting( center, radius ) &&
        notOutForward && notOutTop && notOutBackward && notOutBottom )
        return true;

    if( bbox.Right().IsSphereIntersecting( center, radius ) &&
        notOutForward && notOutTop && notOutBackward && notOutBottom )
        return true;

    if( bbox.Top().IsSphereIntersecting( center, radius ) &&
        notOutForward && notOutLeft && notOutBackward && notOutRight )
        return true;

    if( bbox.Bottom().IsSphereIntersecting( center, radius ) &&
        notOutForward && notOutLeft && notOutBackward && notOutRight )
        return true;

    if( bbox.Forward().IsSphereIntersecting( center, radius ) &&
        notOutLeft && notOutTop && notOutRight && notOutBottom )
        return true;

    if( bbox.Backward().IsSphereIntersecting( center, radius ) &&
        notOutLeft && notOutTop && notOutRight && notOutBottom )
        return true;

    return false;
}

template <typename BoundingBox>
bool IsSphereOutsideBox( const Vec3& center, float radius, const BoundingBox& bbox )
{
    return !( IsSphereInsideBox( center, radius, bbox )
              || IsSphereIntersectBox( center, radius, bbox ) );
}

}

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
                                    const std::vector<Plane>& polyhedron )
{
    return IsSphereInsideConvexPolyhedron( point, 0, polyhedron );
}

bool IsPointOutsideConvexPolyhedron( const Vec3& point,
                                     const std::vector<Plane>& polyhedron )
{
    return !IsPointInsideConvexPolyhedron( point, polyhedron );
}

bool IsSphereInsideConvexPolyhedron( const Vec3& center, float radius,
                                     const std::vector<Plane>& polyhedron )
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
    return IsSphereInsideBox( center, radius, aabb3 );
}

bool IsSphereOutsideAABB3( const Vec3& center, float radius, const AABB3& aabb3 )
{
    return IsSphereOutsideBox( center, radius, aabb3 );
}

bool IsSphereIntersectAABB3( const Vec3& center, float radius, const AABB3& aabb3 )
{
    return IsSphereIntersectBox( center, radius, aabb3 );
}

bool IsSphereInsideOBB3( const Vec3& center, float radius, const OBB3& obb3 )
{
    return IsSphereInsideBox( center, radius, obb3 );
}

bool IsSphereOutsideOBB3( const Vec3& center, float radius, const OBB3& obb3 )
{
    return IsSphereOutsideBox( center, radius, obb3 );
}

bool IsSphereIntersectOBB3( const Vec3& center, float radius, const OBB3& obb3 )
{
    return IsSphereIntersectBox( center, radius, obb3 );
}




}


