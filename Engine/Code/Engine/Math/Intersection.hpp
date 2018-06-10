#pragma once
#include "Engine/Math/Vec3.hpp"

class Plane;
class Segment3;

namespace Geometry
{
//returns false if parallel, with 0 or infinite intersections.
bool LinePlaneIntersect( const Vec3& pointOnLine, const Vec3& lineDir,
                const Vec3& pointOnPlane, const Vec3& planeNormal,
                Vec3& out_intersectionPoint );

bool SegmentPlaneIntersect( const Segment3& s, const Plane& p );

}