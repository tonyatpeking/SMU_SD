#include "Engine/Time/Clock.hpp"

#include "Game/RigidBody.hpp"
#include "Game/GameCommon.hpp"

RigidBody::RigidBody( std::string type )
    : GameObject(type)
{
}

void RigidBody::Update()
{
    GameObject::Update();
    float ds = g_gameClock->GetDeltaSecondsF();
    Transform& trans = GetTransform();
    Vec3 pos = trans.GetWorldPosition();
    trans.SetWorldPosition( pos + m_velocity * ds );
    m_velocity = m_velocity - m_velocity * m_drag * ds;
}
