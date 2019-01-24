#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include "Game/GameplayDefines.hpp"

class NetCube;
class ClientInputs;
class Timer;

class Player
{
public:
	Player();
	~Player();
    bool PopShootTimer();

    NetCube* m_cube = nullptr;
    ClientInputs* m_inputs = nullptr;
    Timer* m_shootTimer = nullptr;
    uint8 m_id;
};
