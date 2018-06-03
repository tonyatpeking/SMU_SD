#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/MeshPrimitive.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Math/Random.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Asteroid.hpp"

Asteroid::Asteroid( float maxHealth )
    : m_maxHealth( maxHealth )
    , m_health( maxHealth )
{
    SetUpRenderable();
}

Asteroid::~Asteroid()
{
}

void Asteroid::HitWithProjectile( const Vec3& projectileVelocity )
{
    m_health = m_health - 1;
    float normalizedHealth = m_health / m_maxHealth;
    Rgba tint = Lerp( Rgba::RED, Rgba::WHITE, normalizedHealth );
    m_renderable->GetMaterial( 0 )->SetTint( tint );

    m_velocity += projectileVelocity * 0.01f;
    if( m_health <= 0 )
    {
        SetShouldDie( true );
    }
}

void Asteroid::SetUpRenderable()
{
    MeshBuilder sphere = MeshPrimitive::MakeUVSphere( m_health );
    m_renderable = new Renderable();
    m_renderable->SetMesh( sphere.MakeMesh() );
    Material* mat = new Material();
    mat->m_specularAmount = 0.7f;
    mat->m_specularPower = 30.f;
    mat->m_shaderPass = new ShaderPass();
    mat->m_shaderPass->m_program = ShaderProgram::CreateOrGetFromFiles(
        "Data/Shaders/lit" );
    m_renderable->SetMaterial( 0, mat );

}


