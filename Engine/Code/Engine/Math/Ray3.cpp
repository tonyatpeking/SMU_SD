#include "Engine/Math/Ray3.hpp"



Ray3::Ray3( const Vec3& point, const Vec3& dir )
    : m_point( point )
    , m_direction( dir )
{
    Normalzie();

}

void Ray3::SetStartEnd( const Vec3& start, const Vec3& end )
{
    m_point = start;
    m_direction = end - start;
    Normalzie();
}

void Ray3::Normalzie()
{
    m_direction.NormalizeAndGetLength();
}
