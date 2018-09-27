#pragma once
#include "Engine/Math/Vec3.hpp"
#include <vector>

class Plane;
class Segment3;
class OBB3;
class AABB3;

namespace Intersect
{
//returns false if parallel, with 0 or infinite intersections.
bool LinePlane( const Vec3& pointOnLine, const Vec3& lineDir,
                const Vec3& pointOnPlane, const Vec3& planeNormal,
                Vec3& out_intersectionPoint );

bool SegmentPlane( const Segment3& s, const Plane& p );
bool SphereSphere( const Vec3& center1, float radius1,
                   const Vec3& center2, float radius2 );
bool PointSphere( const Vec3& point, const Vec3& center1, float radius1 );



// see http://theorangeduck.com/page/correct-box-sphere-intersection
template <typename Hexahedron>
bool IsSphereInsideHexahedron( const Vec3& center, float radius, const Hexahedron& bbox )
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

template <typename Hexahedron>
bool IsSphereIntersectHexahedron( const Vec3& center, float radius, const Hexahedron& bbox )
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

template <typename Hexahedron>
bool IsSphereOutsideHexahedron( const Vec3& center, float radius, const Hexahedron& bbox )
{
    return !( IsSphereInsideHexahedron( center, radius, bbox )
              || IsSphereIntersectHexahedron( center, radius, bbox ) );
}

// Pass in the face planes of the Convex Polyhedron
// they must have normals pointing out
bool IsPointInsideConvexPolyhedron( const Vec3& point, const vector<Plane>& polyhedron );
bool IsPointOutsideConvexPolyhedron( const Vec3& point, const vector<Plane>& polyhedron );
bool IsSphereInsideConvexPolyhedron( const Vec3& center, float radius,
                             const vector<Plane>& polyhedron );

bool IsSphereInsideAABB3( const Vec3& center, float radius, const AABB3& aabb3 );
bool IsSphereOutsideAABB3( const Vec3& center, float radius, const AABB3& aabb3 );
bool IsSphereIntersectAABB3( const Vec3& center, float radius, const AABB3& aabb3 );

bool IsSphereInsideOBB3( const Vec3& center, float radius, const OBB3& obb3 );
bool IsSphereOutsideOBB3( const Vec3& center, float radius, const OBB3& obb3 );
bool IsSphereIntersectOBB3( const Vec3& center, float radius, const OBB3& obb3 );
}