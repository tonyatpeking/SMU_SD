#pragma once
#include "Engine/Math/Vec3.hpp"

namespace Geometry
{
//returns false if parallel, with 0 or infinite intersections.
bool LinePlaneIntersection( const Vec3& pointOnLine, const Vec3& lineDir,
                const Vec3& pointOnPlane, const Vec3& planeNormal,
                Vec3& out_intersectionPoint );

}