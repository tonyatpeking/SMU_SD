#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Time/Timer.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Hive.hpp"
#include "Game/Swarmer.hpp"
#include "Game/Game.hpp"


Swarmer::Swarmer()
{
    CreateRenderable();
}

Swarmer::~Swarmer()
{
    TellHiveIDied();
}

void Swarmer::Update()
{
    Transform& trans = g_game->m_shipHull->GetTransform();
    Vec3 playerPos = trans.GetWorldPosition();
    Vec3 myPos = GetTransform().GetWorldPosition();

    Vec3 dir = playerPos - myPos;
    m_velocity = dir.GetNormalized() * m_speed;


    RigidBody::Update();
}

void Swarmer::CreateRenderable()
{
    SetRenderable( Renderable::MakeCube() );
    m_renderable->GetMaterial( 0 )->SetShaderPass( 0, ShaderPass::GetLitShader() );
    m_transform.SetLocalScale( m_scale );
}

void Swarmer::SetHive( Hive* hive )
{
    m_hive = hive;
}

void Swarmer::TellHiveIDied()
{
    if( m_hive )
        m_hive->SwarmerDied( this );
}
