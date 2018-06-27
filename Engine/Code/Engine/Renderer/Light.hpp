#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/GameObject.hpp"

class Light : public GameObject
{
public:
    Light();
    virtual ~Light();

    float GetContributionToPoint( Vec3 point );

    Rgba m_color = Rgba::WHITE;
    float m_intensity = 1;
    float m_isPointLight = 1;  // 1 for point light, 0 for directional light

    float m_sourceRadius = 1; // the physical size of the light, affects attenuation

    float m_coneInnerDot = -2;
    float m_coneOuterDot = -2;

private:

};
