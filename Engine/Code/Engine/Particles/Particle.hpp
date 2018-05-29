#pragma once

#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"

struct Particle
{
    Vec3 m_position; // local position
    Vec3 m_velocity;
    Vec3 m_acceleration;
    float m_drag = 0;

    Rgba m_startColor = Rgba::WHITE;
    Rgba m_endColor = Rgba::WHITE;

    float m_startSize = 1;
    float m_endSize = 1;

    float m_currentAge = 0;
    float m_maxAge = 1;

    void Update( float ds );

    bool ShouldDie() { return m_currentAge >= m_maxAge; }

    float GetNormalizedAge();

};
