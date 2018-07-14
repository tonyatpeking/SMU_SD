#pragma once
#include <string>
#include "Engine/Core/Types.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/IVec3.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IRange.hpp"
#include "Engine/Math/Range.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/SmartEnum.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/GameObjectManager.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Time/TweenSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Renderer/MeshPrimitive.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Core/Profiler.hpp"

#include "Game/GameState_Playing.hpp"

#define LOAD_FROM_BLACKBOARD( attrName )                                                            \
{                                                                                                   \
    attrName = g_blackboard->GetValue( STRINGIZE_NO_PREFIX( attrName ), attrName );              \
}

SMART_ENUM
(
    RenderMode,

    NORMAL,
    ALWAYS,
    NEVER
)


// Engine systems
class  Renderer;
extern Renderer* g_renderer;
class  InputSystem;
extern InputSystem* g_input;
class  AudioSystem;
extern AudioSystem* g_audio;
class Window;
extern Window* g_window;
class  Console;
extern Console* g_console;

// App systems
class  App;
extern App* g_app;
class  Camera;
extern Camera* g_UICamera;
class  Blackboard;
extern Blackboard* g_blackboard;
class  Config;
extern Config* g_config;

// Game systems
class  Game;
extern Game* g_game;
class ForwardRenderingPath;
extern ForwardRenderingPath* g_forwardRenderingPath;
class RenderSceneGraph;
extern RenderSceneGraph* g_renderSceneGraph;
class GameObjectManager;
extern GameObjectManager* g_gameObjectManager;


class XMLDocument;
extern XMLDocument* g_configXML;
class XMLElement;
extern XMLElement* g_lpcAnimSetXML;


//Clocks
class Clock;
extern Clock* g_realtimeClock; //does not slow if framerate slows
extern Clock* g_appClock;   //slows if framerate slows
extern Clock* g_UIClock;
extern Clock* g_gameClock;
//These global tweenSystems are only intended for tweening objects with global lifetime
class TweenSystem;
extern TweenSystem* g_UITweenSystem;
extern TweenSystem* g_gameTweenSystem;



// Global control variables
extern bool g_devModeOn;
extern bool g_fullMapMode;

SMART_ENUM(
    GameFlag,

    runTests
)

extern std::map<GameFlag, bool> g_flags;

constexpr char* CONFIG_PATH = "Data/GameConfig.xml";
constexpr char* SHADER_TEST_RELOAD = "Data/Shaders/testReload";
constexpr char* SHADER_ERROR = "Data/Shaders/error";
constexpr char* SHADER_UV = "Data/Shaders/showUV";
constexpr char* TEST_TEXTURE = "Data/Images/test_opengl.png";


bool GetFlag( const String& flag );
bool GetFlag( GameFlag flag );
void SetFlag( const String& flag, bool set = true );
void SetFlag( GameFlag, bool set = true );
void ToggleFlag( const String& flag );
void ToggleFlag( GameFlag flag );

class Config
{
public:

    void LoadConfigFromBlackboard();
    // Definitions
    String tileDefPath;
    String mapDefPath;
    String actorDefPath;
    String projectileDefPath;
    String portalDefPath;
    String adventureDefPath;
    String itemDefPath;


    // Key bindings
    char keyPause = '\0';
    char keySlowTime = '\0';

    String appName = "";

    // Camera
    Vec2 orthoSysSize = Vec2::ZEROS;
    float orthoSysWidth = 0.f;
    float orthoSysHeight = 0.f;
    AABB2 orthoSysBounds;
    float clientAspect = 1.f;
    float cameraZoomTilesVertical = 1.f;
    Rgba overlayColor = Rgba::BLACK;
    Rgba backgroundColor = Rgba::DIM_GRAY;

    // Time
    float framsPerSecond = 60.f;
    float maxDeltaSeconds = 0.033f;
    float speedOfTimeNormal = 1.f;
    float speedOfTimeSlow = 0.1f;
    float speedOfTimeFast = 5.f;

    // Console
    float consoleHeightRatio = 1.f;

    // Tile
    float tileWidth = 1.f;
    float halfTileWidth = 0.5f;
    int raycastStepsPerTile = 10;

    // Entity
    float entityStandingStillEpsilon = 0.001f;

    // Player
    IVec2 playerStartCoords = IVec2( 3, 3 );

    // Texture
    String testTexturePath = "";
    String fontPath = "";
    IVec2 fontSprtiesheetLayout = IVec2( 16, 16 );

    // Dev mode
    int numDevCircleSides = 30;
    Rgba velocityColor = Rgba::YELLOW;
    Rgba physicsRadiusColor = Rgba::CYAN;
    Rgba cosmeticRadiusColor = Rgba::MAGENTA;

    // Sounds
    String soundAttract = "";
    String soundGameplay= "";
    String soundPause = "";
    String soundVictory = "";
    String soundDeath = "";

    // Gamestates
    Rgba transitionColor = Rgba::BLACK;
    float deathExitTime = 0.3f;
    float deathEnterTime = 0.3f;

    float attractExitTime = 0.3f;
    float attractEnterTime = 0.3f;
    Rgba attractBackgroundColor = Rgba::OLIVE;

    float gameplayExitTime = 0.3f;
    float gameplayEnterTime = 0.3f;

    float pauseEnterTime = 0.3f;
    float pauseExitTime = 0.3f;
    RenderMode healthBarRenderMode = RenderMode::NORMAL;
};

