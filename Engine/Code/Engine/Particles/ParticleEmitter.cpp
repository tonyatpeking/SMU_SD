#include "Engine/Particles/ParticleEmitter.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Renderer/Mesh.hpp"

ParticleEmitter::ParticleEmitter( Clock* clock )
    : m_clock( clock )
    , m_spawnTimer( clock, m_spawnRate )
{
    SetupRenderable();
    m_builder.SetVertexType<VertexPCU>();
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::PreRender( Camera* camera )
{
    RegenMesh( camera );
}

void ParticleEmitter::Update()
{
    GameObject::Update();

    if( m_spawnRate > 0 )
        SpawnParticlesBasedOnTimer();

    float ds = m_clock->GetDeltaSecondsF();

    for( auto& particle : m_particles )
        particle.Update( ds );


    for( int partIdx = (int) m_particles.size() - 1; partIdx >= 0; --partIdx )
    {
        if( m_particles[partIdx].ShouldDie() )
            ContainerUtils::EraseAtIndexFast( m_particles, partIdx );
    }


    if( ( m_particles.size() == 0 ) && ( m_spawnRate == 0 ) )
        SetShouldDie( true );
}

void ParticleEmitter::SpawnParticlesBasedOnTimer()
{
    uint count = m_spawnTimer.PopAllLaps();
    SpawnParticles( count );
}

void ParticleEmitter::SetupRenderable()
{
    m_renderable = new Renderable();

    Material* material = new Material();
    material->SetShaderPass( 0, ShaderPass::GetAdditiveShader() );
    Texture* texture = Texture::GetParticleTexture();
    material->SetDiffuse( texture );
    m_renderable->SetMaterial( 0, material );
}

void ParticleEmitter::RegenMesh( Camera* cameraToFace )
{

    Vec3 right = cameraToFace->GetTransform().GetRight();
    Vec3 up = cameraToFace->GetTransform().GetUp();

    uint vertCount = (uint) m_particles.size();
    AABB2 bounds = AABB2( Vec2::ZEROS, 1, 1 );

    m_builder.Clear();
    m_builder.Reserve( vertCount * 4, vertCount * 6 );
    m_builder.BeginSubMesh();

    for( uint particleIdx = 0; particleIdx < vertCount; ++particleIdx )
    {
        Particle& particle = m_particles[particleIdx];
        Vec3 pos = particle.m_position;
        float normAge = particle.GetNormalizedAge();
        Rgba color = Lerp( particle.m_startColor, particle.m_endColor, normAge );
        float size = Lerp( particle.m_startSize, particle.m_endSize, normAge );

        m_builder.AddQuad( pos, right, up, bounds * size, color );
    }
    m_builder.EndSubMesh();

    Mesh* mesh = m_builder.MakeMesh();
    delete m_renderable->GetMesh();
    m_renderable->SetMesh( mesh );

}

void ParticleEmitter::SpawnParticle()
{
    m_particles.emplace_back( Particle{} );
    Particle& particle = m_particles.back();
    if( m_simulateInWorldSpace )
    {
        particle.m_position = m_spawnPointTransform.GetWorldPosition();
    }

    if( m_particleSpawnCB )
        m_particleSpawnCB( particle );
}

void ParticleEmitter::SpawnParticles( uint count )
{
    m_particles.reserve( m_particles.size() + count );
    for( uint idx = 0; idx < count; ++idx )
    {
        SpawnParticle();
    }
}

void ParticleEmitter::SetSpawnRate( float spawnRate )
{
    bool shouldResetTimer = false;
    if( m_spawnRate <= 0
        || spawnRate <= 0
        || ( m_spawnRate / spawnRate ) > 2.f
        || ( m_spawnRate / spawnRate ) < 0.5f )
    {
        shouldResetTimer = true;
    }
    m_spawnRate = spawnRate;
    if( spawnRate > 0 )
    {
        float interval = 1 / spawnRate;
        if( shouldResetTimer )
            m_spawnTimer.Reset();
        m_spawnTimer.SetLapTime( interval );
    }
}

void ParticleEmitter::Burst( int min, int max )
{
    int burstCount = Random::IntInRange( min, max );
    SpawnParticles( burstCount );
}

void ParticleEmitter::SetParticleSpawnCB( void( *particleSpawnCB )( Particle& ) )
{
    m_particleSpawnCB = particleSpawnCB;
}

void ParticleEmitter::SetParentWorldSpace( GameObject* parent )
{
    m_spawnPointTransform.SetParent( &parent->GetTransform() );
}

void ParticleEmitter::KillDeadParticles()
{
    for( int particleIdx = (int) m_particles.size() - 1; particleIdx >= 0; --particleIdx )
    {
        if( m_particles[particleIdx].ShouldDie() )
        {
            ContainerUtils::EraseAtIndexFast( m_particles, particleIdx );
        }
    }
}
