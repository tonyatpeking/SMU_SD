#include "Engine/Core/Window.hpp"

#include "Game/GameState_Lobby.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

GameState_Lobby::GameState_Lobby()
    : GameState( GameStateType::LOBBY )
{

}

GameState_Lobby::~GameState_Lobby()
{

}

void GameState_Lobby::Update()
{
    GameState::Update();
}

void GameState_Lobby::Render() const
{
    GameState::Render();

    g_renderer->UseUICamera();
    g_renderer->DrawAABB( AABB2::MakeBoundsFromDimensions( g_window->GetDimensions() ), g_config->overlayColor );
    Vec2 screenCenter = (Vec2) g_window->GetDimensions() * 0.5f;
    float textHeight = 30.f;
    g_renderer->DrawText2D( "Click or Press Enter to Continue", screenCenter, textHeight );
}

void GameState_Lobby::OnEnter()
{
    GameState::OnEnter();
}

void GameState_Lobby::OnExit()
{
    GameState::OnExit();
}

void GameState_Lobby::ProcessInput()
{
    GameState::ProcessInput();
    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_ENTER )
        || g_input->WasKeyJustPressed( InputSystem::MOUSE_LEFT ) )
        g_game->ChangeState( GameStateType::PLAYING );
}


