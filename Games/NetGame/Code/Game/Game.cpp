#include <functional>

#include "Engine/Core/Rgba.hpp"
#include "Engine/Image/HeatMap.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/FileIO/XmlUtils.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Image/Image.hpp"
#include "Engine/FileIO/Blackboard.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Math/CubicSpline.hpp"
#include "Engine/Math/Range.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/FileIO/IOUtils.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Core/CommandSystem.hpp"
#include "Engine/UI/Menu.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Renderer/UBO.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Python/PythonInterpreter.hpp"
#include "Engine/Core/EngineCommands.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Log/Logger.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/Core/EngineCommonC.hpp"

// Net
#include "Engine/Net/UDPTest.hpp"
#include "Engine/Net/EngineNetMessages.hpp"
#include "Engine/Net/Net.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Game/GameNetMessages.hpp"

#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameCommands.hpp"
#include "Game/GameState_Playing.hpp"
#include "Game/GameState_Loading.hpp"
#include "Game/GameState_MainMenu.hpp"
#include "Game/Tests.hpp"

#include "ThirdParty/pybind11/embed.h"
namespace py = pybind11;
using namespace pybind11::literals;


Game::Game()
{
    Initialize();
}

Game::~Game()
{
    DeleteTextures();
}

void Game::Update()
{
    PROFILER_SCOPED();

    CheckSwapToNewState();

    ProcessInput();

    m_currentGameState->Update();

    UpdatePause();

    UDPTest::GetInstance()->Update();

    UpdateNetworkTest();
}

void Game::Render() const
{
    PROFILER_SCOPED();

    if( m_currentGameState->GetType() == GameStateType::PLAYING )
        g_forwardRenderingPath->Render( g_renderSceneGraph );

    m_currentGameState->Render();

    RenderPause();
}

void Game::Initialize()
{
#ifdef _DEBUG
    SetFlag( GameFlag::runTests );
#endif // _DEBUG

    g_input->LockCursor( true );
    g_input->ClipCursor( true );
    g_input->ShowCursor( false );

    LoadTextures();
    LoadShaders();

    g_renderer->SetBackGroundColor( g_config->backgroundColor );
    g_forwardRenderingPath = new ForwardRenderingPath( g_renderer );
    g_renderSceneGraph = RenderSceneGraph::GetDefault();
    g_gameObjectManager = GameObjectManager::GetDefault();

    LoadSounds();

    EngineCommands::RegisterAllCommands();

    GameCommands::RegisterAllCommands();

    ChangeState( GameStateType::LOADING );

    if( GetFlag( GameFlag::runTests ) )
        Tests::RunTests();

    MakePauseMenu();

    PythonInterpreter::GetInstance()->Start();
    PythonInterpreter* shell = PythonInterpreter::GetInstance();


    UNUSED( shell );

}

void Game::ChangeState( GameStateType gameStateType, float timeTillTransition /*= 0.f */ )
{
    m_transitioningToState = gameStateType;
    m_timeTillTransition = timeTillTransition;
}



void Game::CheckSwapToNewState()
{
    float realDS = g_appClock->GetDeltaSecondsF();
    if( GameStateType::INVALID == m_transitioningToState )
        return;

    m_timeTillTransition -= realDS;

    if( m_timeTillTransition <= 0.f )
    {
        if( m_currentGameState )
        {
            m_currentGameState->OnExit();
            delete m_currentGameState;
        }
        m_currentGameState = GameState::MakeGameState( m_transitioningToState );
        m_transitioningToState = GameStateType::INVALID;
        m_timeTillTransition = 0.f;
        m_currentGameState->OnEnter();
    }
}

void Game::LoadTextures()
{
    //Texture* terrainTexture = g_renderer->CreateOrGetTexture( TERRAIN_IMG_PATH );
    //g_terrainSpriteSheet = new SpriteSheet( *terrainTexture, TERRAIN_SPRITESHEET_LAYOUT );
    g_renderer->CreateOrGetTexture( TEST_TEXTURE );
}

void Game::LoadSounds()
{
    //     g_audio->CreateSound( g_config->soundAttract );
    //     g_audio->CreateSound( g_config->soundGameplay );
    //     g_audio->CreateSound( g_config->soundPause );
    //     g_audio->CreateSound( g_config->soundVictory );
}

void Game::LoadConfigs()
{
    if( g_blackboard )
        delete g_blackboard;
    g_blackboard = new Blackboard();
    g_blackboard->PopulateFromFile( CONFIG_PATH );

    if( g_config )
        delete g_config;
    g_config = new Config();
    g_config->LoadConfigFromBlackboard();

}

void Game::LoadShaders()
{
    //g_renderer->CreateOrUpdateShaderProgramFromPath( SHADER_ERROR );
    ShaderProgram::CreateOrGetFromFiles( SHADER_UV );
    ShaderProgram::CreateOrGetFromFiles( "Data/Shaders/grayscale" );
    ShaderProgram::CreateOrGetFromFiles( "Data/Shaders/fog" );
    ShaderProgram::CreateOrGetFromFiles( "Data/Shaders/cubeBorders" );

    string maxLightDefine = Stringf( "MAX_LIGHTS = (%i)", MAX_LIGHTS );
    ShaderProgram::CreateOrGetFromFiles( "Data/Shaders/lit", maxLightDefine );
}

void Game::DeleteTextures()
{
    //delete g_terrainSpriteSheet;
    //g_terrainSpriteSheet = nullptr;
}


void Game::ProcessInput()
{
    XboxController* controller = g_input->GetFirstConnectedController();
    //-----------------------------------------------------------------------------------------------
    // app controls

    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_TILDE ) )
    {
        g_console->ToggleActive();


        if( g_console->IsActive() )
        {
            g_input->PushCursorSettings();
            g_input->ShowAndUnlockCursor();
        }
        else
        {
            g_input->PopCursorSettings();
        }
    }

    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_ESCAPE )
        || ( controller != nullptr && controller->WasKeyJustPressed( XboxController::XBOX_KEY_BACK ) ) )
    {
        if( g_console->IsActive() )
        {
            if( g_console->HasInput() )
                g_console->ClearInput();
            else
                g_console->SetActive( false );
        }
    }

    if( g_console->IsActive() )
        return;

    if( IsPaused() )
        m_pauseMenu->ProcessMouseInput();

    //     if( g_input->WasKeyJustPressed( g_config->keyPause )
    //         || ( controller != nullptr && controller->WasKeyJustPressed( XboxController::XBOX_KEY_START ) )
    //         )
    //     {
    //         if( IsPaused() )
    //             SetIsPaused( false );
    //         else
    //             SetIsPaused( true );
    //     }

    if( g_input->WasKeyJustPressed( g_config->keySlowTime ) )
        SetGameTimeScale( g_config->speedOfTimeSlow );

    if( g_input->WasKeyJustReleased( g_config->keySlowTime ) )
        SetGameTimeScale( g_config->speedOfTimeNormal );

    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_F1 ) )
        g_devModeOn = !g_devModeOn;	// Dev Mode

    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_F2 ) )
        g_fullMapMode = !g_fullMapMode;	// Full map mode

    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_F3 ) ) // Max Speed mode
        ToggleSpeedUp();

    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_F4 ) ) // Toggle debug render modes
        ToggleDebugRenderModes();

    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_F5 ) ) // Restart game
    {
        auto fn = [&]() {ReloadDataAndRestartGame(); };
        CallFunctinoNTimes( fn, 1 ); // Set this to 100 to test for memory leaks when reloading definitions
    }

    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_F8 ) ) // ScreenShot
        g_renderer->TakeScreenshot();

    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_F9 ) ) // Reload Shaders
        ShaderProgram::UpdateAllShadersFromDisk();



    if( IsPaused() )
        return;

    m_currentGameState->ProcessInput();

}

void Game::SetGameTimeScale( float timeScale )
{
    g_gameClock->SetTimeScale( timeScale );
}


void Game::SetIsPaused( bool paused )
{
    g_gameClock->SetPause( paused );
    if( m_currentGameState->GetType() != GameStateType::PLAYING )
        g_gameClock->SetPause( false );
}

bool Game::IsPaused() const
{
    return g_gameClock->IsPaused();
}


void Game::RenderPause() const
{
    if( !g_gameClock->IsPaused() && m_pauseEffectCurrentTime < 0 )
        return;


    //ShaderProgram* grayscaleShader = ShaderProgram::GetProgram( "Data/Shaders/grayscale" );

    struct InstanceData
    {
        float percent;
    } instanceData;

    instanceData.percent = m_pauseEffectCurrentTime / m_pauseEffectTotalTime;
    UniformBuffer instanceBuffer;
    instanceBuffer.Set( instanceData );
    instanceBuffer.BindToSlot( UBO::USER_BINDING );
    //g_renderer->ApplyEffect( grayscaleShader );
    g_renderer->UseUICamera();

    m_pauseMenu->Render();
}

void Game::MakePauseMenu()
{
    m_pauseMenu = new Menu( g_renderer, g_input );
    AABB2 bounds = AABB2( 0, 0, 500, 500 );
    bounds.SetCenter( (Vec2) g_window->GetDimensions() / 2 );

    m_pauseMenu->m_bounds = bounds;

    m_pauseMenu->m_entryHeight = 30;

    m_pauseMenu->AddEntry( "Resume", [this]() {SetIsPaused( false ); } );
    m_pauseMenu->AddEntry( "Exit", [this]() {
        ChangeState( GameStateType::MAIN_MENU );
        SetIsPaused( false ); } );

    m_pauseMenu->RefreshEntryBounds();

}

void Game::UpdatePause()
{
    if( g_gameClock->IsPaused() && m_pauseEffectCurrentTime <= m_pauseEffectTotalTime )
        m_pauseEffectCurrentTime += g_UIClock->GetDeltaSecondsF();

    if( !g_gameClock->IsPaused() && m_pauseEffectCurrentTime >= 0 )
        m_pauseEffectCurrentTime -= g_UIClock->GetDeltaSecondsF();
}

void Game::ToggleDebugRenderModes()
{
    const int modes = 3; //normal, wireframe, lit
    static int currentMode = 0;

    ++currentMode;
    if( currentMode == modes )
        currentMode = 0;

    switch( currentMode )
    {
    case 0:
        g_renderer->SetOverrideShader( nullptr );
        break;
    case 1:
        g_renderer->SetDebugWireframeShader();
        break;
    case 2:
        g_renderer->SetDebugLightingShader();
        break;
    default:
        g_renderer->SetOverrideShader( nullptr );
        break;
    }

}



void Game::StartGame()
{

}

void Game::EndGame()
{

}

void Game::ReloadDataAndRestartGame()
{
    g_console->Clear();
    EndGame();
    StartGame();
}

void Game::ToggleSpeedUp()
{
    static bool isSpeedUp = false;
    isSpeedUp = !isSpeedUp;
    if( isSpeedUp )
        SetGameTimeScale( g_config->speedOfTimeFast );
    else
        SetGameTimeScale( g_config->speedOfTimeNormal );
}

//--------------------------------------------------------------------------------------
// TEST
// UnreliableTest Vars
namespace
{
uint s_idx;
uint s_totalCount = 0;
uint s_currentCount = 0;
Timer* timer = nullptr;
eNetworkTestType s_networkTestType;
}

void Game::StartNetworkTest( uint idx, uint count, eNetworkTestType testType )
{
    s_idx = idx;
    s_totalCount = count;
    s_currentCount = 0;
    delete timer;
    timer = new Timer( Clock::GetRealTimeClock(), 0.1f );
    s_networkTestType = testType;
}

void Game::UpdateNetworkTest()
{
    if( s_currentCount >= s_totalCount )
        return;
    if( timer->PopOneLap() )
    {
        NetMessage* msg = nullptr;
        switch( s_networkTestType )
        {
        case eNetworkTestType::UNRELIABLE:
            msg = GameNetMessages::Compose_UnreliableTest(
                s_currentCount, s_totalCount );
            break;
        case eNetworkTestType::RELIABLE:
            msg = GameNetMessages::Compose_ReliableTest(
                s_currentCount, s_totalCount );
            break;
        case eNetworkTestType::SEQUENCE:
            msg = GameNetMessages::Compose_SequenceTest(
                s_currentCount, s_totalCount );
            break;
        default:
            break;
        }

        NetSession::GetDefault()->SendToConnection( (uint8) s_idx, msg );
        s_currentCount += 1;
    }
}