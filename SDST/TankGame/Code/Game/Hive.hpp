#pragma once
#include "Engine/Math/Vec3.hpp"

#include "Game/RigidBody.hpp"

class Swarmer;
class Timer;

class Hive : public RigidBody
{
public:
    Hive();
    virtual ~Hive();

    virtual void Update() override;
    virtual void OnFirstUpdate() override;

    void CreateRenderable();
    void SpawnSwarmer();
    void SwarmerDied( Swarmer* swarmer );
    void TellSwarmersIDied();

    void TakeDamage( float damage );

private:

    std::vector< Swarmer* > m_swarmers;
    int m_maxSwarmerCount = 90;
    int m_initialSwarmerCount = 30;
    float m_spawnInterval = 0.5f;
    Vec3 m_hiveScale = Vec3( 10, 40, 10 );
    Timer* m_spawnTimer = nullptr;
    float m_health = 1.f;
};
