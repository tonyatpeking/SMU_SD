#include "Engine/Renderer/OrbitCamera.hpp"



void OrbitCamera::SetTarget( const Vec3& target )
{
    m_targetPos = target;
    CalculateMatrices();
}

void OrbitCamera::SetSphericalCoord( float radius, float elevation, float azimuth )
{
    m_radius = radius;
    m_elevation = elevation;
    m_azimuth = azimuth;
    CalculateMatrices();
}

void OrbitCamera::CalculateMatrices()
{
    Vec3 camPos = SphericalToCartesian( m_radius, m_elevation, m_azimuth );
    camPos += m_targetPos;
    m_transform.SetWorldPosition( camPos );
    m_transform.LookAt( m_targetPos );
}
