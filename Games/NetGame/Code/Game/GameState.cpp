#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Time/TweenSystem.hpp"

#include "Game/GameState.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameState_Playing.hpp"
#include "Game/GameState_Loading.hpp"
#include "Game/GameState_MainMenu.hpp"
#include "Game/GameState_Victory.hpp"
#include "Game/GameState_Lobby.hpp"

GameState::GameState( GameStateType gameStateType )
    : m_gameStateType( gameStateType )
{
    m_tweenSystem = new TweenSystem();
}

GameState* GameState::MakeGameState( GameStateType gameStateType )
{
    switch( gameStateType )
    {
    case GameStateType::PLAYING:
        return new GameState_Playing();
    case GameStateType::LOADING:
        return new GameState_Loading();
    case GameStateType::MAIN_MENU:
        return new GameState_MainMenu();
    case GameStateType::VICTORY:
        return new GameState_Victory();
    case GameStateType::LOBBY:
        return new GameState_Lobby();
    default:
        LOG_FATAL( "Unimplemented GameState: " + GameStateType::ToString( gameStateType ) );
        return nullptr;
    }
}

GameState::~GameState()
{
    delete m_tweenSystem;
}

void GameState::Update()
{
    float ds = g_gameClock->GetDeltaSecondsF();
    m_tweenSystem->Update( ds );
}

void GameState::Render() const
{


}

void GameState::OnEnter()
{
    m_UITimeOfEnter = g_UIClock->GetTimeSinceStartupF();
    g_console->Print( ToString() );

}

void GameState::OnExit()
{

}

void GameState::ProcessInput()
{

}

String GameState::ToString()
{
    return GameStateType::ToString( m_gameStateType );
}
