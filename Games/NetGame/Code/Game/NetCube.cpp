#include "Game/NetCube.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/MeshPrimitive.hpp"
#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Time/Clock.hpp"



map<uint16, NetCube*> NetCube::s_allCubes;
uint16 NetCube::s_nextID = 0;

NetCube::NetCube( const Vec3& position,
                  const Vec3& euler,
                  const Vec3& scale,
                  const Rgba& color,
                  uint16 netID )
    : GameObject( "NetCube" )
{
    MeshBuilder mb = MeshPrimitive::MakeCube( Vec3::ONES, Rgba::WHITE, Vec3::ZEROS );
    Renderable* r = new Renderable();
    r->SetMesh( mb.MakeMesh() );
    r->GetMaterial( 0 )->SetShaderPass( 0, ShaderPass::GetLitShader() );
    r->GetMaterial( 0 )->SetTint( color );
    SetRenderable( r );
    m_transform.SetLocalPosition( position );
    m_transform.SetLocalEuler( euler );
    m_transform.SetLocalScale( scale );

    SetTargetPosition( position );
    SetTargetEuler( euler );
    SetTargetScale( scale );
    SetTargetColor( color );

    m_netID = netID;
    s_allCubes[m_netID] = this;
}

NetCube::~NetCube()
{
    s_allCubes.erase( m_netID );
}

void NetCube::Update()
{
    m_targetPosition += m_velocity * BULLET_SPEED * g_gameClock->GetDeltaSecondsF();
    Vec3 oldPos = m_transform.GetLocalPosition();
    Vec3 displacement = m_targetPosition - oldPos;
    float length = displacement.NormalizeAndGetLength();
    length = Clampf( length, 0,
                     PLAYER_MOVE_SPEED * g_gameClock->GetDeltaSecondsF() );

    Vec3 newPos = oldPos + displacement * length;
    newPos += m_velocity * BULLET_SPEED * g_gameClock->GetDeltaSecondsF();
    m_transform.SetLocalPosition( newPos );
    m_transform.SetLocalEuler( m_targetEuler );
    m_transform.SetLocalScale( m_targetScale );
    m_renderable->GetMaterial( 0 )->SetTint( m_targetColor );
}

void NetCube::SetTargetPosition( const Vec3& position )
{
    m_targetPosition = position;
}

void NetCube::SetTargetEuler( const Vec3& euler )
{
    m_targetEuler = euler;
}

void NetCube::SetTargetScale( const Vec3& scale )
{
    m_targetScale = scale;
}

void NetCube::SetTargetColor( const Rgba& color )
{
    m_targetColor = color;
}

Rgba NetCube::GetColor()
{
    return m_renderable->GetMaterial( 0 )->m_tint;
}

void NetCube::SetColor( const Rgba& color )
{
    m_renderable->GetMaterial( 0 )->SetTint( color );
}

NetCube* NetCube::GetNetCube( uint16 netID )
{
    if( ContainerUtils::ContainsKey( s_allCubes, netID ) )
        return s_allCubes[netID];
    return nullptr;
}

void NetCube::MarkForDestroy( uint16 netID )
{
    NetCube* cube = GetNetCube( netID );
    if( cube )
        cube->SetShouldDie( true );
}

uint16 NetCube::GetNextFreeNetID()
{
    for( uint16 i = 0; i < MAX_NET_ID_COUNT; ++i )
    {
        s_nextID += 1;
        if( !ContainerUtils::ContainsKey( s_allCubes, s_nextID ) )
            return s_nextID;
    }
    return INVALID_NET_ID;
}

