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
    Hive * m_hive;

    Vec3 m_scale = Vec3( 2, 2, 2 );
    float m_speed = 10.f;
};
