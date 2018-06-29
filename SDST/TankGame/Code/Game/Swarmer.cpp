#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/MeshPrimitive.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Hive.hpp"
#include "Game/Swarmer.hpp"
#include "Game/Game.hpp"

namespace
{
// swarmer data
static float s_attractWeight = 4.f;
static float s_attractRadius = 10.f;
static float s_attractRadiusSq = s_attractRadius * s_attractRadius;

static float s_repulseWeight = 400.f;
static float s_repulseRadius = 2.f;
static float s_repulseRadiusSq = s_repulseRadius * s_repulseRadius;

static float s_alignWeight = 4.f;
static float s_alignRadius = 7.f;
static float s_alignRadiusSq = s_alignRadius * s_alignRadius;

static float s_seekWeight = 20.f;

static float s_speed = 20.f;
static float s_turnSpeed = 220.f;
}


Swarmer::Swarmer()
    : RigidBody( "Swarmer" )
{
    CreateRenderable();
}

Swarmer::~Swarmer()
{
    TellHiveIDied();
}

void Swarmer::Update()
{
    SetTargetFromSwarming();
    TurnTowardTarget();



    RigidBody::Update();
}

void Swarmer::CreateRenderable()
{
    MeshBuilder mbBall =
        MeshPrimitive::MakeUVSphere( m_radius, 5, 8, Rgba::WHITE, Vec3::UP * 0.5f );
    MeshBuilder mbNose =
        MeshPrimitive::MakeCube( Vec3( 0.4f, 0.4f, 1.4f ), Rgba::YELLOW,
                                 Vec3( 0.0f, 1.0f, 1.4f ) );

    MeshBuilder mb{};
    mb.BeginSubMesh();
    mb.AddBuilder( mbBall );
    mb.AddBuilder( mbNose );
    mb.EndSubMesh();

    SetRenderable( new Renderable() );
    m_renderable->SetMesh( mb.MakeMesh() );
    m_renderable->GetMaterial( 0 )->SetShaderPass( 0, ShaderPass::GetLitShader() );
    //m_transform.SetLocalScale( m_scale );
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

void Swarmer::TakeDamage( float damage )
{
    m_health -= damage;

    if( m_health < 0 )
        SetShouldDie( true );

    m_health = Clampf01( m_health );
    Rgba tint = Lerp( Rgba::RED, Rgba::WHITE, m_health );
    m_renderable->GetMaterial( 0 )->SetTint( tint );

}

void Swarmer::SetTargetFromSwarming()
{
    Vec2 attractVec, repulseVec, alignVec, seekVec;

    Vec2 posMe = Vec2::MakeFromXZ( GetTransform().GetWorldPosition() );

    GameObjects allSwarmers = g_gameObjectManager->GetObjectsOfType( "Swarmer" );
    for( int i = 0; i < allSwarmers.size(); ++i )
    {
        Swarmer* swarmer = (Swarmer*) allSwarmers[i];
        if( swarmer != this )
        {
            Vec2 posOther = Vec2::MakeFromXZ( swarmer->GetTransform().GetWorldPosition() );
            float distSq = Vec2::GetDistanceSquared( posMe, posOther );
            // Too far
            if( distSq >= s_attractRadiusSq )
                continue;
            // Attract
            if( distSq < s_attractRadiusSq )
                attractVec += posOther - posMe;
            // Repulse
            if( distSq < s_repulseRadiusSq )
            {
                repulseVec += posMe - posOther;
                if( repulseVec.SnapToZero() )
                {
                    repulseVec += Random::Vec2InRange(
                        Vec2( -0.1f, -0.1f ), Vec2( -0.1f, -0.1f ) );
                }
            }
            // Align
            if( distSq < s_alignRadiusSq )
                alignVec += swarmer->GetFacing();
        }
    }
    seekVec = g_game->m_shipHull->GetTransform().GetWorldPosition().ToVec2XZ() - posMe;
    m_target = attractVec * s_attractWeight
        + repulseVec * s_repulseWeight
        + alignVec * s_alignWeight
        + seekVec * s_seekWeight;
    m_target.Normalize();
    m_target.SnapToZero();
    if( m_target == Vec2::ZEROS )
        m_target = Vec2::ONE_ZERO;
}

void Swarmer::TurnTowardTarget()
{
    float t = g_gameClock->GetDeltaSecondsF() * s_turnSpeed;
    m_facing = TurnToward( m_facing, m_target, t );
    m_facing.Normalize();
    m_velocity = m_facing.ToVec3XZ() * s_speed;
    GetTransform().LookAt( GetTransform().GetWorldPosition() + m_velocity );

}


