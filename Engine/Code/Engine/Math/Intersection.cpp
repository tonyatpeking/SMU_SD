#include "Engine/Math/Intersection.hpp"



bool Geometry::LinePlaneIntersection( const Vec3& pointOnLine, const Vec3& lineDir,
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
