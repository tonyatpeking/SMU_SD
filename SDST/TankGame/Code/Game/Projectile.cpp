#include "Engine/Time/Clock.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/MeshPrimitive.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Vertex.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Projectile.hpp"


Projectile::Projectile( const Rgba& tint /*= Rgba::ORANGE */ )
    : m_tint( tint )
{
    SetUpRenderable();
    MakeLight();
}

Projectile::~Projectile()
{
    delete m_renderable->GetMesh();
    DestroyLight();
}

void Projectile::Update()
{
    RigidBody::Update();
    float ds = g_gameClock->GetDeltaSecondsF();
    m_age += ds;

    if( m_age > m_maxAge )
        SetShouldDie( true );
}

void Projectile::SetUpRenderable()
{
    MeshBuilder quad = MeshPrimitive::MakeQuad( AABB2( -2, -0.2f, 2, 0.2f ), m_tint );
    Mat4 rotate = Mat4::MakeRotationEuler( Vec3( 0, 90, 0 ) );
    quad.TransformAllVerts( rotate );
    rotate = Mat4::MakeRotationEuler( Vec3( 0, 0, 60 ) );

    MeshBuilder mb{};
    mb.SetVertexType<VertexPCU>();
    mb.BeginSubMesh();
    mb.AddBuilder( quad );

    quad.TransformAllVerts( rotate );
    mb.AddBuilder( quad );

    quad.TransformAllVerts( rotate );
    mb.AddBuilder( quad );

    mb.EndSubMesh();

    m_renderable = new Renderable();
    m_renderable->SetMesh( mb.MakeMesh() );

    Material* material = new Material();
    material->SetShaderPass( 0, ShaderPass::GetAdditiveShader() );
    Texture* texture = g_renderer->CreateOrGetTexture( "Data/Images/particle.png" );
    material->SetDiffuse( texture );
    m_renderable->SetMaterial( 0, material );
}

void Projectile::MakeLight()
{
    m_light = new Light();
    m_light->m_color = m_tint;
    m_light->m_intensity = 10;
    m_light->m_sourceRadius = 1.f;
    m_light->GetTransform().SetLocalToParent(m_transform.GetLocalToParent());
    m_light->SetParent( this );

    g_renderSceneGraph->AddLight( m_light );
}

void Projectile::DestroyLight()
{
    g_renderSceneGraph->RemoveLight( m_light );
    delete m_light;
}
