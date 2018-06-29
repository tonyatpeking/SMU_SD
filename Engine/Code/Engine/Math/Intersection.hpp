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

// Pass in the face planes of the Convex Polyhedron
// they must have normals pointing out
bool IsPointInsideConvexPolyhedron( const Vec3& point, const std::vector<Plane>& polyhedron );
bool IsPointOutsideConvexPolyhedron( const Vec3& point, const std::vector<Plane>& polyhedron );
bool IsSphereInsideConvexPolyhedron( const Vec3& center, float radius,
                             const std::vector<Plane>& polyhedron );

bool IsSphereInsideAABB3( const Vec3& center, float radius, const AABB3& aabb3 );
bool IsSphereOutsideAABB3( const Vec3& center, float radius, const AABB3& aabb3 );
bool IsSphereIntersectAABB3( const Vec3& center, float radius, const AABB3& aabb3 );

bool IsSphereInsideOBB3( const Vec3& center, float radius, const OBB3& obb3 );
bool IsSphereOutsideOBB3( const Vec3& center, float radius, const OBB3& obb3 );
bool IsSphereIntersectOBB3( const Vec3& center, float radius, const OBB3& obb3 );
}