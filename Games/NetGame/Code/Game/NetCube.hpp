#pragma once
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Game/GameplayDefines.hpp"
#include "Game/GameCommon.hpp"

#define MAX_NET_ID_COUNT 1000
#define INVALID_NET_ID ((uint16)(~0))

class NetCube : public GameObject
{
public:
    NetCube(
        const Vec3& position,
        const Vec3& euler,
        const Vec3& scale,
        const Rgba& color,
        uint16 netID
    );
    virtual ~NetCube();
    void Update() override;

    void SetTargetPosition( const Vec3& position );
    void SetTargetEuler( const Vec3& euler );
    void SetTargetScale( const Vec3& scale );
    void SetTargetColor( const Rgba& color );

    Rgba GetColor();
    void SetColor( const Rgba& color );


    // Net ID
    uint16 GetNetID() { return m_netID; };
    static NetCube* GetNetCube( uint16 netID );
    static void MarkForDestroy( uint16 netID );
    static uint16 GetNextFreeNetID();

    static map<uint16, NetCube*>& GetAllCubes() { return s_allCubes; };

public:

    static map<uint16, NetCube*> s_allCubes;
    static uint16 s_nextID;

    uint16 m_netID;
    uint8 m_factionID;

    Vec3 m_targetPosition;
    Vec3 m_targetEuler;
    Vec3 m_targetScale;
    Rgba m_targetColor;

    Vec3 m_direction = Vec3::UP;
    Vec3 m_velocity = Vec3::ZEROS;

    float m_timeToLive = BULLET_LIFETIME;
};
