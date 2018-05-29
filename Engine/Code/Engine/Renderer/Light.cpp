#include "Engine/Renderer/Light.hpp"



float Light::GetContributionToPoint( Vec3 point )
{
    float dist = ( m_transform.GetWorldPosition() - point ).GetLength();
    float temp = ( dist / m_sourceRadius ) + 1;
    float attenuation = 1.f / ( temp* temp );
    return attenuation * m_intensity;
}
