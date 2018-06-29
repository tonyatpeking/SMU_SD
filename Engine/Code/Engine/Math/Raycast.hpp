#pragma once
#include "Engine/Math/RaycastHit3.hpp"
#include "Engine/Math/Vec2.hpp"

class Ray3;
class Plane;
class IVec2;
class OBB3;
class AABB3;

namespace Raycast
{
RaycastHit3 ToPlane( const Ray3& ray, const Plane& plane );
RaycastHit3 ToAABB3( const Ray3& ray, const AABB3& bbox );
RaycastHit3 ToOBB3( const Ray3& ray, const OBB3& obb );
//return the closest raycast, if both are the same, prefer ray1
RaycastHit3 GetClosest( const RaycastHit3& hit1, const RaycastHit3& hit2 );

};
