#include "Game/GameState_Loading.hpp"
#include "Game/Game.hpp"

GameState_Loading::GameState_Loading()
    : GameState( GameStateType::LOADING )
{

}

GameState_Loading::~GameState_Loading()
{

}

void GameState_Loading::Update()
{
    GameState::Update();
}

void GameState_Loading::Render() const
{
    GameState::Render();
}

void GameState_Loading::OnEnter()
{
    GameState::OnEnter();


    g_game->ChangeState( GameStateType::MAIN_MENU, 0.0f );

}

void GameState_Loading::OnExit()
{
    GameState::OnExit();
}

void GameState_Loading::ProcessInput()
{
    GameState::ProcessInput();
}


