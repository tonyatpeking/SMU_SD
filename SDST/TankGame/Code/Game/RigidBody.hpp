#pragma once
#include "Engine/Core/GameObject.hpp"

class RigidBody : public GameObject
{
public:
	RigidBody(){};
	virtual ~RigidBody(){};
    virtual void Update() override;

    Vec3 m_velocity;
    float m_drag = 0.f;

private:

};
