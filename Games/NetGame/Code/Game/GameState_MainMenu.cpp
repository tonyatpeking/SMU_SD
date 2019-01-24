#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/UI/Menu.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Time/TweenSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Core/RuntimeVars.hpp"
#include "Engine/Core/CommandSystem.hpp"

#include "Game/GameState_MainMenu.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"

GameState_MainMenu::GameState_MainMenu()
    : GameState( GameStateType::MAIN_MENU )

{

}

GameState_MainMenu::~GameState_MainMenu()
{
    if( m_menu )
        delete m_menu;
}

void GameState_MainMenu::Update()
{
    GameState::Update();
    if( RuntimeVars::IsBoolSet( AUTO_JOIN ) )
    {
        CommandSystem::DefaultCommandSystem()->RunCommand( "join" );
        RuntimeVars::EraseVar( AUTO_JOIN );
    }
    if( NetSession::GetDefault()->m_state == eSessionState::READY )
        StartGame();
}

void GameState_MainMenu::Render() const
{
    GameState::Render();
//
//
    g_renderer->UseUICamera();
    g_renderer->DrawAABB( AABB2::MakeBoundsFromDimensions( g_window->GetDimensions() ), g_config->overlayColor );


    Vec2 screenCenter = (Vec2) g_window->GetDimensions() * 0.5f;

    string text = "Net Game";
    Vec2 textPosition = screenCenter + Vec2( 0.f, 250.f );
    float textHeight = 70.f;
    float time = g_UIClock->GetTimeSinceStartupF();
    float timeOffset = 0.06f;
    float interval = 2.f;
    float flashDuration = 0.2f;
    float nextGlyphOffsetSeconds = 0.03f;
    g_renderer->DrawText2DFlashWave(
        text,
        textPosition + Vec2( m_textOffset, 0 ),
        textHeight,
        nullptr,
        g_UIClock->GetTimeSinceStartupF(),
        Rgba::WHITE,
        Rgba::GRAY,
        interval,
        flashDuration,
        nextGlyphOffsetSeconds );


    string textStart   = " Press Start to Start!               ";
    string textLabel   = " F4 Debug shader                     ";
    string textMove    = " Move:       WASD                    ";
    string textAim     = " Look:       Mouse                   ";
    string textShoot   = " ";
    string textAuthor  = "                        Tony Yu      ";

    textPosition = textPosition - Vec2( 300.f, 150.f );
    Vec2 menuPosition = textPosition + Vec2( 800.f, 0.f );
    m_menu->Render();


    textHeight = 20.f;
    time -= timeOffset * 4.f;
    BitmapFont* font = nullptr;
    g_renderer->DrawText2DFlashWave( textStart, textPosition, textHeight, font, time, Rgba::WHITE,
                                     Rgba::BLACK, interval, flashDuration, nextGlyphOffsetSeconds );


    textPosition = textPosition - Vec2( 0.f, 150.f );
    Vec2 textOffset = Vec2( 0.f, 70.f );
    time -= timeOffset;
    g_renderer->DrawText2DFlashWave( textLabel, textPosition, textHeight, font, time, Rgba::WHITE,
                                     Rgba::BLACK, interval, flashDuration, nextGlyphOffsetSeconds );

    textPosition = textPosition - textOffset;
    time -= timeOffset;
    g_renderer->DrawText2DFlashWave( textMove, textPosition, textHeight, font, time, Rgba::WHITE,
                                     Rgba::BLACK, interval, flashDuration, nextGlyphOffsetSeconds );

    textPosition = textPosition - textOffset;
    time -= timeOffset;
    g_renderer->DrawText2DFlashWave( textAim, textPosition, textHeight, font, time, Rgba::WHITE,
                                     Rgba::BLACK, interval, flashDuration, nextGlyphOffsetSeconds );

    textPosition = textPosition - textOffset;
    time -= timeOffset;
    g_renderer->DrawText2DFlashWave( textShoot, textPosition, textHeight, font, time, Rgba::WHITE,
                                     Rgba::BLACK, interval, flashDuration, nextGlyphOffsetSeconds );


    textPosition = textPosition - Vec2( 0, 150.f );
    textHeight = 30.f;
    time -= timeOffset;
    g_renderer->DrawText2DFlashWave( textAuthor, textPosition, textHeight, font, time, Rgba::WHITE,
                                     Rgba::BLACK, interval, flashDuration, nextGlyphOffsetSeconds );

}

void GameState_MainMenu::OnEnter()
{
    GameState::OnEnter();

    //StartGame();
    g_input->LockCursor( false );
    g_input->ClipCursor( false );
    g_input->ShowCursor( true );

    m_menu = new Menu( g_renderer, g_input );
    AABB2 windowBounds = g_window->GetWindowBounds();
    m_menu->m_bounds = windowBounds.RangeMap01ToBounds( 0.5f, 0.f, 1.f, 0.6f );
    m_menu->AddEntry( "Start", [this]() { StartGame(); } );
    m_menu->AddEntry( "Exit", []() { g_app->OnQuitRequested(); } );

    m_menu->RefreshEntryBounds();

    Tween* tween = m_tweenSystem->MakeTween( m_textOffset, 0.f, 2.f );
    tween->SetTweenMode( TweenMode::PINGPONG );
    tween->SetEasing( Easing::BounceStop );

}

void GameState_MainMenu::OnExit()
{
    GameState::OnExit();
}

void GameState_MainMenu::ProcessInput()
{
    GameState::ProcessInput();
    m_menu->ProcessMouseInput();

    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_ESCAPE ) )
        g_app->OnQuitRequested();
}

void GameState_MainMenu::StartGame()
{
    g_game->ChangeState( GameStateType::PLAYING );
}


