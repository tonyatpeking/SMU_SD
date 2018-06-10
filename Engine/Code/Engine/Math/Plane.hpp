#pragma once
#include "Engine/Math/Vec3.hpp"

struct RaycastHit3;

class Plane
{
public:
    Plane() {};
    ~Plane() {};

    Plane( const Vec3& norm, const Vec3& pos )
        : distance( Dot( norm, pos ) )
        , normal( norm )
    {};
    Plane( const Vec3& a, const Vec3& b, const Vec3& c );

    // Mutators
    void FlipNormal();


    // Point plane tests
    float GetDistance( const Vec3& pos ) const;
    bool OnPositiveSide( const Vec3& pos ) const;



    Vec3 normal = Vec3::UP;
    // distance to the origin
    float distance = 0;

};
