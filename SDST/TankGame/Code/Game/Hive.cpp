#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Time/Timer.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Hive.hpp"
#include "Game/Swarmer.hpp"

Hive::Hive()
    : RigidBody( "Hive" )
{
    CreateRenderable();
}

Hive::~Hive()
{
    TellSwarmersIDied();
    delete m_spawnTimer;
}

void Hive::Update()
{
    RigidBody::Update();

    int numToSpawn = m_spawnTimer->PopAllLaps();

    for( int i = 0; i < numToSpawn; ++i )
    {
        if( m_swarmers.size() >= m_maxSwarmerCount )
            break;
        SpawnSwarmer();
    }
}

void Hive::OnFirstUpdate()
{
    m_spawnTimer = new Timer( g_gameClock, m_spawnInterval );
    RigidBody::OnFirstUpdate();
    for( int i = 0; i < m_initialSwarmerCount; ++i )
    {
        SpawnSwarmer();
    }
}

void Hive::CreateRenderable()
{
    SetRenderable( Renderable::MakeCube() );
    m_renderable->GetMaterial( 0 )->SetShaderPass( 0, ShaderPass::GetLitShader() );
    m_transform.SetLocalScale( m_hiveScale );
}

void Hive::SpawnSwarmer()
{
    Swarmer* swarmer = new Swarmer();
    Vec3 pos = GetTransform().GetWorldPosition();
    swarmer->GetTransform().SetWorldPosition( GetTransform().GetWorldPosition() );
    swarmer->SetHive( this );

    m_swarmers.push_back( swarmer );
}

void Hive::SwarmerDied( Swarmer* swarmer )
{
    ContainerUtils::EraseOneValue( m_swarmers, swarmer );
}

void Hive::TellSwarmersIDied()
{
    for( int i = 0; i < m_swarmers.size(); ++i )
    {
        m_swarmers[i]->SetHive( nullptr );
    }
}

