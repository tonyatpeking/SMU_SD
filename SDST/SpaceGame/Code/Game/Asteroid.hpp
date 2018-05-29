#pragma once
#include "Game/RigidBody.hpp"

class Asteroid : public RigidBody
{
public:
    Asteroid( float maxHealth );
    ~Asteroid();

    void HitWithProjectile( const Vec3& projectileVelocity );
    void SetUpRenderable();

    // also used as radius
    float m_maxHealth;
    float m_health;

private:

};
