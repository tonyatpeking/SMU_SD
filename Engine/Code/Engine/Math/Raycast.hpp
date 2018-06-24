#pragma once
#include "Engine/Math/RaycastHit3.hpp"
#include "Engine/Math/Vec2.hpp"

class Ray3;
class Plane;
class IVec2;
class AABB3;

namespace Raycast
{
RaycastHit3 ToPlane( const Ray3& ray, const Plane& plane );
RaycastHit3 ToAABB3( const Ray3& ray, const AABB3& bbox );



};
