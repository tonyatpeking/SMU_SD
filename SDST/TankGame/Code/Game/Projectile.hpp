#pragma once
#include "Engine/Core/GameObject.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Game/RigidBody.hpp"

class Light;

class Projectile : public RigidBody
{
public:
    Projectile( const Rgba& tint = Rgba(200,100,20,200) );
    virtual ~Projectile();

    virtual void Update() override;

    void SetUpRenderable();
    void MakeLight();
    void DestroyLight();


    Rgba m_tint;

    Light* m_light;

    float m_age = 0;
    const float MAX_AGE = 1;
};

