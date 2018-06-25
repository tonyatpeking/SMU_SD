#pragma once
#include "Engine/Math/Vec3.hpp"

#include "Game/RigidBody.hpp"

class Swarmer;

class Hive : public RigidBody
{
public:
    Hive();
    virtual ~Hive();

    virtual void Update() override;

    void CreateRenderable();
    void SpawnSwarmer();
    void SwarmerDied( Swarmer* swarmer );
    void TellSwarmersIDied();
private:

    std::vector< Swarmer* > m_swarmers;
    int m_maxSwarmerCount = 20;
    int m_initialSwarmerCount = 10;
    float m_spawnInterval = 0.5f;
    Vec3 m_hiveScale = Vec3( 10, 40, 10 );
};
