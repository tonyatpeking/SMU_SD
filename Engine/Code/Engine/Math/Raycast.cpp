#include "Engine/Math/Raycast.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Plane.hpp"
#include "Engine/Math/Ray3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/IVec2.hpp"

namespace Raycast
{

RaycastHit3 ToPlane( const Ray3& ray, const Plane& plane )
{
    float vel = Dot( ray.direction, plane.normal );
    if( AlmostZero( vel ) )
        return RaycastHit3::NO_HIT;

    float dir = Dot( plane.normal, ray.direction );
    float dist = plane.GetDistance( ray.start );
    if( ( dir * dist ) > 0.0f )
    {
        return RaycastHit3::NO_HIT;
    }

    float time = -dist / vel;
    return RaycastHit3( time, ray.Evaluate( time ), plane.normal );
}

RaycastHit3 ToAABB3( const Ray3& ray, const AABB3& bbox )
{
    //                7-------6
    //               /|      /|
    //              3-|-----2 |
    //              | 4-----|-5
    //              |/   o  |/
    //              0-------1

// https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms

//Graphics Gems algorithm:

    // Using your ray's direction vector, determine which 3 of the 6 candidate
    // planes would be hit first. If your (unnormalized) ray direction vector is
    // (-1, 1, -1), then the 3 planes that are possible to be hit are +x, -y, and +z.

    // Of the 3 candidate planes, find the t value for the intersection for each.
    // Accept the plane that gets the largest t value as being the plane that got hit,
    // and check that the hit is within the box.

    Vec3 dirInv = Vec3( 1, 1, 1 ) / ray.direction;
    Vec3 normal = Vec3::UP;

    float t1 = ( bbox.mins.x - ray.start.x ) * dirInv.x;
    float t2 = ( bbox.maxs.x - ray.start.x ) * dirInv.x;
    float t3 = ( bbox.mins.y - ray.start.y ) * dirInv.y;
    float t4 = ( bbox.maxs.y - ray.start.y ) * dirInv.y;
    float t5 = ( bbox.mins.z - ray.start.z ) * dirInv.z;
    float t6 = ( bbox.maxs.z - ray.start.z ) * dirInv.z;


    float tmin = Maxf( Maxf( Minf( t1, t2 ), Minf( t3, t4 ) ), Minf( t5, t6 ) );
    float tmax = Minf( Minf( Maxf( t1, t2 ), Maxf( t3, t4 ) ), Maxf( t5, t6 ) );

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if( tmax < 0 )
    {
        return RaycastHit3::NO_HIT;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if( tmin > tmax )
    {
        return RaycastHit3::NO_HIT;
    }

    if( tmin == t1 )
        normal = Vec3::LEFT;
    if( tmin == t2 )
        normal = Vec3::RIGHT;
    if( tmin == t3 )
        normal = Vec3::DOWN;
    if( tmin == t4 )
        normal = Vec3::UP;
    if( tmin == t5 )
        normal = Vec3::BACKWARD;
    if( tmin == t6 )
        normal = Vec3::FORWARD;

    return RaycastHit3( tmin, ray.Evaluate( tmin ), normal );

}


}

