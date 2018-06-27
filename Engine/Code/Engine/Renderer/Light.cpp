#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"


Light::Light()
    : GameObject( "Light" )
{
    // this is needed because only the base class SetScene will be called
}

Light::~Light()
{

}

float Light::GetContributionToPoint( Vec3 point )
{
    float dist = ( m_transform.GetWorldPosition() - point ).GetLength();
    float temp = ( dist / m_sourceRadius ) + 1;
    float attenuation = 1.f / ( temp* temp );
    return attenuation * m_intensity;
}
