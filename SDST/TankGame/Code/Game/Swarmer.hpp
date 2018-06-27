#pragma once
#include "Engine/Math/Vec3.hpp"

#include "Game/RigidBody.hpp"

class Hive;

class Swarmer : public RigidBody
{
public:
    Swarmer();
    virtual ~Swarmer();

    virtual void Update() override;

    void CreateRenderable();


    void SetHive( Hive* hive );
    void TellHiveIDied();
private:

    // Flocking behavior
    void SetTargetFromSwarming();
    void TurnTowardTarget();
    Vec2 GetFacing() { return m_facing; };

    Hive * m_hive;

    Vec2 m_target;
    Vec2 m_facing = Vec2::ONE_ZERO;
    Vec3 m_scale = Vec3( 2, 2, 2 );


};
