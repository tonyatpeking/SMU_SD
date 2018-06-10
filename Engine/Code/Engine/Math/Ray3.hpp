#pragma once
#include "Engine/Math/Vec3.hpp"

class Ray3
{
public:
    Ray3() {};
    Ray3( const Vec3& point, const Vec3& dir );
    ~Ray3() {};
    void SetStartEnd( const Vec3& start, const Vec3& end );
    void Normalzie();
    Vec3 Evaluate( float t ) const;

public:
    Vec3 start;
    Vec3 direction;

private:

};
