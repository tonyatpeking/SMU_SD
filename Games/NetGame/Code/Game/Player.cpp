#include "Engine/Time/Clock.hpp"
#include "Engine/Time/Timer.hpp"

#include "Game/Player.hpp"
#include "Game/ClientInputs.hpp"
#include "Game/NetCube.hpp"

Player::Player()
{
    m_inputs = new ClientInputs();
    m_shootTimer = new Timer( Clock::GetRealTimeClock(), PLAYER_SHOOT_INTERVAL );
}

Player::~Player()
{
    delete m_inputs;
    delete m_shootTimer;
    if( m_cube )
        m_cube->SetShouldDie( true );
}

bool Player::PopShootTimer()
{
    return m_shootTimer->PopAllLaps() != 0;
}
