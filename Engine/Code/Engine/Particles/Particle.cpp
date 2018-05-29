#include "Engine/Particles/Particle.hpp"
#include "Engine/Math/MathUtils.hpp"


void Particle::Update( float ds )
{
    m_velocity -= m_velocity * m_drag * ds;
    m_velocity += ds * m_acceleration;
    m_position += ds * m_velocity;
    m_currentAge += ds;
}

float Particle::GetNormalizedAge()
{
    return Clampf01( m_currentAge / m_maxAge);
}
