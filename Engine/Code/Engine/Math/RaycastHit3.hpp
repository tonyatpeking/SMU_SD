#pragma once
#include "Engine/Math/Vec3.hpp"

class GameObject;

struct RaycastHit3
{
public:

    static const RaycastHit3 NO_HIT;

    RaycastHit3() {};
    ~RaycastHit3() {};

    RaycastHit3( float distance, Vec3 position, Vec3 normal )
        : m_hit( true )
        , m_distance( distance )
        , m_position( position )
        , m_normal( normal )
    {}

    bool m_hit = false;
    GameObject* m_gameObject = nullptr;
    float m_distance = 0;
    Vec3 m_position;
    Vec3 m_normal;

};
