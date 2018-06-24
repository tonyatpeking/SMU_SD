#pragma once
#include "Engine/Math/Vec2.hpp"

class Ray3;

class Ray2
{
public:
    static Ray2 FromRay3XZ( const Ray3& ray3 );

    Ray2() {};
    Ray2( const Vec2& point, const Vec2& dir );
    ~Ray2() {};
    void SetStartEnd( const Vec2& start, const Vec2& end );
    void Normalzie();
    Vec2 Evaluate( float t ) const;
    bool IsValid() { return direction != Vec2::ZEROS; };

public:
    Vec2 start;
    Vec2 direction;
};
