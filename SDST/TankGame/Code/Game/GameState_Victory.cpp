#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/UI/Menu.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Input/InputSystem.hpp"

#include "Game/GameState_Victory.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"

GameState_Victory::GameState_Victory()
    : GameState( GameStateType::VICTORY )

{

}

GameState_Victory::~GameState_Victory()
{

}

void GameState_Victory::Update()
{
    GameState::Update();


}

void GameState_Victory::Render() const
{
    GameState::Render();

    g_renderer->UseUICamera();
    g_renderer->DrawAABB( AABB2::MakeBoundsFromDimensions( g_window->GetDimensions() ), g_config->overlayColor );

    Vec2 screenCenter = Vec2( g_config->orthoSysWidth / 2.f, g_config->orthoSysHeight / 2.f );

    String text = Stringf("Team %i Wins!", g_game->m_victoryTeam);
    Vec2 textPosition = screenCenter + Vec2( 0.f, 250.f );
    float textHeight = 70.f;
    float interval = 2.f;
    float flashDuration = 0.2f;
    float nextGlyphOffsetSeconds = 0.03f;
    g_renderer->DrawText2DFlashWave(
        text,
        textPosition,
        textHeight,
        nullptr,
        g_UIClock->GetTimeSinceStartupF(),
        Rgba::WHITE,
        Rgba::GRAY,
        interval,
        flashDuration,
        nextGlyphOffsetSeconds );
}

void GameState_Victory::OnEnter()
{
    GameState::OnEnter();
}

void GameState_Victory::OnExit()
{
    GameState::OnExit();
}

void GameState_Victory::ProcessInput()
{
    GameState::ProcessInput();
    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_ESCAPE ) )
        g_game->ChangeState( GameStateType::MAIN_MENU );
}

