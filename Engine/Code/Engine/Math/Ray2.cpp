#include "Engine/Math/Ray2.hpp"
#include "Engine/Math/Ray3.hpp"


Ray2 Ray2::FromRay3XZ( const Ray3& ray3 )
{
    Ray2 ray2{};
    ray2.start = Vec2::MakeFromXZ( ray3.start );
    ray2.direction = Vec2::MakeFromXZ( ray3.direction );
    return ray2;
}

Ray2::Ray2( const Vec2& point, const Vec2& dir )
    : start( point )
    , direction( dir )
{
    Normalzie();
}

void Ray2::SetStartEnd( const Vec2& startPoint, const Vec2& end )
{
    start = startPoint;
    direction = end - start;
    Normalzie();
}

void Ray2::Normalzie()
{
    direction.NormalizeAndGetLength();
}

Vec2 Ray2::Evaluate( float t ) const
{
    return start + direction * t;
}
