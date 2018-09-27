#pragma once
#include "Engine/Particles/Particle.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Math/Range.hpp"
#include "Engine/Math/IRange.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

class MeshBuilder;
class Clock;

class ParticleEmitter : public GameObject
{
public:
    ParticleEmitter( Clock* clock );
    ~ParticleEmitter();

    virtual void PreRender( Camera* camera ) override;
    virtual void Update() override;

    void SpawnParticle();
    void SpawnParticles( uint count );

    void SetSpawnRate( float spawnRate );

    void Burst( int min, int max );

    // use this call back to customize the particle initial values
    void SetParticleSpawnCB( void( *particleSpawnCB )( Particle& ) );
    void SetParentWorldSpace( GameObject* parent );

private:

    void SpawnParticlesBasedOnTimer();
    void SetupRenderable();
    void RegenMesh( Camera* cameraToFace );
    void KillDeadParticles();

public:

    void( *m_particleSpawnCB )( Particle& ) = nullptr;

    MeshBuilder m_builder;

    vector<Particle> m_particles;

    Clock* m_clock = nullptr;

    float m_spawnRate = 1;
    Timer m_spawnTimer;

    bool m_simulateInWorldSpace = true;
    Transform m_spawnPointTransform;

};
