#include "Engine/Math/Intersection.hpp"
#include "Engine/Math/Segment3.hpp"
#include "Engine/Math/Plane.hpp"

bool Geometry::LinePlaneIntersect( const Vec3& pointOnLine, const Vec3& lineDir,
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

bool Geometry::SegmentPlaneIntersect( const Segment3& s, const Plane& p )
{
    float d0 = p.GetDistance( s.start );
    float d1 = p.GetDistance( s.end );

    return ( d0 * d1 ) <= 0.0f;
}
