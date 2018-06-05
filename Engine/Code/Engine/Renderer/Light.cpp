#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"


Light::Light()
{
    // this is needed because only the base class SetScene will be called
    if( m_scene )
        m_scene->AddLight( this );
}

Light::~Light()
{
    if( m_scene )
        m_scene->RemoveLight( this );
}

void Light::SetScene( RenderSceneGraph* scene )
{
    if( m_scene )
        m_scene->RemoveLight( this );

    GameObject::SetScene( scene );

    if( m_scene )
        m_scene->AddLight( this );
}

float Light::GetContributionToPoint( Vec3 point )
{
    float dist = ( m_transform.GetWorldPosition() - point ).GetLength();
    float temp = ( dist / m_sourceRadius ) + 1;
    float attenuation = 1.f / ( temp* temp );
    return attenuation * m_intensity;
}
