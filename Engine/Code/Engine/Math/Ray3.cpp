#include "Engine/Math/Ray3.hpp"



Ray3::Ray3( const Vec3& point, const Vec3& dir )
    : start( point )
    , direction( dir )
{
    Normalzie();

}

void Ray3::SetStartEnd( const Vec3& startPoint, const Vec3& end )
{
    start = startPoint;
    direction = end - start;
    Normalzie();
}

void Ray3::Normalzie()
{
    direction.NormalizeAndGetLength();
}

Vec3 Ray3::Evaluate( float t ) const
{
    return start + direction * t;
}
