#include <functional>

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/HeatMap.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Blackboard.hpp"
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
#include "Engine/IO/IOUtils.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Core/CommandSystem.hpp"
#include "Engine/UI/Menu.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Renderer/UBO.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Core/PythonInterpreter.hpp"

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

PYBIND11_EMBEDDED_MODULE( fast_calc, m ) {
    // `m` is a `py::module` which is used to bind functions and classes
    m.def( "add", []( int i, int j ) {
        return i + j;
    } );
    m.def( "rand", []( int i, int j ) {
        return Random::IntInRange( i, j );
    } );
}





Game::Game()
    : m_overlayMusicPlaybackID( MISSING_SOUND_ID )
    , m_gameplayMusicPlaybackID( MISSING_SOUND_ID )
{
    Initialize();
}

Game::~Game()
{
    DeleteTextures();
}

void Game::Update()
{

    CheckSwapToNewState();

    ProcessInput();

    Tests::UpdateTests();

    m_currentGameState->Update();

    UpdatePause();
}

void Game::Render() const
{
    if( m_currentGameState->GetType() == GameStateType::PLAYING )
        g_forwardRenderingPath->Render( g_renderSceneGraph );

    m_currentGameState->Render();

    RenderPause();

    if( GetFlag( GameFlag::runTests ) )
        Tests::RenderTests();
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
    g_forwardRenderingPath = new ForwardRenderingPath(g_renderer);
    g_renderSceneGraph = RenderSceneGraph::GetDefault();
    g_gameObjectManager = GameObjectManager::GetDefault();

    LoadSounds();

    GameCommands::RegisterAllCommands();

    ChangeState( GameStateType::LOADING );

    if( GetFlag( GameFlag::runTests ) )
        Tests::RunTests();

    MakePauseMenu();

    //py::scoped_interpreter guard{}; // start the interpreter and keep it alive

    PythonInterpreter::GetInstance()->Start();
    PythonInterpreter* shell = PythonInterpreter::GetInstance();

    shell->PushToShell( "2**123" );

    std::string s = shell->ReadFromShell();

    //py::object os = py::module::import( "os" );
    float a = 42.f;
    py::object obj = py::float_(a);
    float x = obj.cast<float>(); // may throw cast_error

    py::object scope = py::module::import( "__main__" ).attr( "__dict__" );

    // Evaluate an isolated expression
    int result = py::eval( "10 + 10", scope ).cast<int>();



//     py::object makedirs = os.attr( "makedirs" );
//     makedirs( "/tmp/path/to/somewhere1" );
    //py::print( "Hello, World!" ); // use the Python API

    py::exec( R"(
        kwargs = dict(name="World", number=42)
        message = "Hello, {name}! The answer is {number}".format(**kwargs)
        print( "********" + message)
    )" );


    auto fast_calc = py::module::import( "fast_calc" );
    result = fast_calc.attr( "rand" )( 1, 200 ).cast<int>();

    py::exec( R"(
        import sys
        sys.path.append( 'D:\\Guildhall\\SpecialTopic\\SDST\\TankGame\\Run_Win32' )
    )" );

    auto py_module = py::module::import( "test" );
    result = py_module.attr( "rnd" ).cast<int>();

    //assert( result == 3 );

    int i = 0;

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

void Game::ToggleTests()
{

}

void Game::LoadTextures()
{
    //Texture* terrainTexture = g_renderer->CreateOrGetTexture( TERRAIN_IMG_PATH );
    //g_terrainSpriteSheet = new SpriteSheet( *terrainTexture, TERRAIN_SPRITESHEET_LAYOUT );
    g_renderer->CreateOrGetTexture( TEST_TEXTURE );
}

void Game::LoadSounds()
{
    g_audio->CreateSound( g_config->soundAttract );
    g_audio->CreateSound( g_config->soundGameplay );
    g_audio->CreateSound( g_config->soundPause );
    g_audio->CreateSound( g_config->soundVictory );
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

    String maxLightDefine = Stringf( "MAX_LIGHTS = (%i)", MAX_LIGHTS );
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

        // save mouse state
        static bool savedClip = false;
        static bool savedLock = false;
        static bool savedShow = true;

        if( g_console->IsActive() )
        {
            savedClip = g_input->m_clipCursor;
            savedLock = g_input->m_lockCursor;
            savedShow = g_input->m_showCursor;
            g_input->LockCursor( false );
            g_input->ClipCursor( false );
            g_input->ShowCursor( true );
        }
        else
        {
            g_input->LockCursor( savedLock );
            g_input->ClipCursor( savedClip );
            g_input->ShowCursor( savedShow );
        }
    }

    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_F7 ) )
        ToggleTests();

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

    if( g_input->WasKeyJustPressed( g_config->keyPause )
        || ( controller != nullptr && controller->WasKeyJustPressed( XboxController::XBOX_KEY_START ) )
        )
    {
        if( IsPaused() )
            SetIsPaused( false );
        else
            SetIsPaused( true );
    }

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
        g_renderer->SetDebugShader( nullptr );
        break;
    case 1:
        g_renderer->SetDebugWireframeShader();
        break;
    case 2:
        g_renderer->SetDebugLightingShader();
        break;
    default:
        g_renderer->SetDebugShader( nullptr );
        break;
    }

}

void Game::StartGame()
{
    Random::InitSeed();
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




