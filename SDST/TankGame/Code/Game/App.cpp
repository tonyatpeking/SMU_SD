#include "Engine/Input/InputSystem.hpp"
#include "Engine/Time/TweenSystem.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Blackboard.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Renderer/TextMeshBuilder.hpp"
#include "Engine/Renderer/DebugRender.hpp"

#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"


App::App()
{
    g_realtimeClock = new Clock();
    g_appClock = new Clock();
    g_UIClock = new Clock( g_appClock );
    g_gameClock = new Clock( g_appClock );

    g_renderer = new Renderer( g_window );
    g_renderer->SetDefaultFont( g_config->fontPath );
    g_renderer->SetClocks( g_gameClock, g_appClock );

    DebugRender::Startup( g_renderer );
    DebugRender::SetClock( g_appClock );


    //instead of awkwardly asking renderer for the font, have a resource manager instead
    TextMeshBuilder::SetDefaultFont( g_renderer->DefaultFont() );

    g_input = new InputSystem( g_window );

    g_console = new Console( g_renderer, g_input );
    Console::SetDefaultConsole( g_console );
    g_console->SetSizeRatio( g_config->consoleHeightRatio );

    g_audio = new AudioSystem();

    g_UITweenSystem = new TweenSystem();
    g_gameTweenSystem = new TweenSystem();

    g_game = new Game();
}

App::~App()
{
    DebugRender::Shutdown();

    delete g_game;
    g_game = nullptr;

    delete g_audio;
    g_audio = nullptr;

    delete g_console;
    g_console = nullptr;

    delete g_input;
    g_input = nullptr;

    delete g_renderer;
    g_renderer = nullptr;
}

void App::RunFrame()
{
    static float lastTime = (float) TimeUtils::GetCurrentTimeSeconds();
    float currentTime = (float) TimeUtils::GetCurrentTimeSeconds();
    float deltaSeconds = currentTime - lastTime;
    g_realtimeClock->Update( deltaSeconds );

    deltaSeconds = Clampf( deltaSeconds, 0.f, g_config->maxDeltaSeconds );
    lastTime = currentTime;
    g_appClock->Update( deltaSeconds );

    g_audio->BeginFrame();
    g_renderer->BeginFrame();
    g_input->BeginFrame();

    g_gameTweenSystem->Update( g_gameClock->GetDeltaSecondsF() );
    g_UITweenSystem->Update( g_UIClock->GetDeltaSecondsF() );

    g_game->Update();
    g_game->Render();

    DebugRender::UpdateAndRender();

    g_console->Update( deltaSeconds );

    g_console->Render();

    g_input->EndFrame();
    g_renderer->EndFrame();
    g_audio->EndFrame();
}

void App::OnQuitRequested()
{
    m_isQuitting = true;
}

bool App::IsQuitting() const
{
    return m_isQuitting;
}
