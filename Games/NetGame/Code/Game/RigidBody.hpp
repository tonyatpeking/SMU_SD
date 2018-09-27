#pragma once
#include <string>
#include "Engine/GameObject/GameObject.hpp"

class RigidBody : public GameObject
{
public:
	RigidBody(string type);
	virtual ~RigidBody(){};
    virtual void Update() override;

    Vec3 m_velocity;
    float m_drag = 0.f;
    float m_mass = 1.f;
private:

};
