#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Blackboard.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

// Engine systems
Renderer* g_renderer = nullptr;
InputSystem* g_input = nullptr;
AudioSystem* g_audio = nullptr;
Window* g_window = nullptr;
Console* g_console = nullptr;

// App systems
App* g_app = nullptr;
Camera* g_UICamera = nullptr;
Blackboard* g_blackboard = nullptr;
Config* g_config = nullptr;

// Game systems
Game* g_game = nullptr;
//Camera* g_theCamera = nullptr;
Encounter* g_encounter = nullptr;
GameState_Playing* g_encounterState = nullptr;
Actor* g_currentActor = nullptr;

ForwardRenderingPath* g_forwardRenderingPath = nullptr;
RenderSceneGraph* g_renderSceneGraph = nullptr;
GameObjectManager* g_gameObjectManager = nullptr;


XMLDocument* g_configXML = nullptr;
XMLElement* g_lpcAnimSetXML = nullptr;

//Clocks
Clock* g_realtimeClock = nullptr;
Clock* g_appClock = nullptr; //slows if framerate slows

Clock* g_UIClock = nullptr;
Clock* g_gameClock = nullptr;

TweenSystem* g_UITweenSystem = nullptr;
TweenSystem* g_gameTweenSystem = nullptr;

bool g_devModeOn = false;
bool g_fullMapMode = false;

std::map<GameFlag, bool> g_flags;

GameFlag StringToFlag( String str )
{
    StringUtils::RemoveOuterWhitespace( str );
    GameFlag flag = GameFlag::FromString( str );
    return flag;
}


bool GetFlag( const String& flagStr )
{
    GameFlag flag = StringToFlag( flagStr );
    return g_flags[flag];
}

bool GetFlag( GameFlag flag )
{
    return g_flags[flag];
}

void SetFlag( const String& flagStr, bool set /*= true */ )
{
    GameFlag flag = StringToFlag( flagStr );
    g_flags[flag] = set;
}

void SetFlag( GameFlag flag, bool set /*= true */ )
{
    g_flags[flag] = set;
}

void ToggleFlag( GameFlag flag )
{
    if( g_flags[flag] )
        g_flags[flag] = false;
    else
        g_flags[flag] = true;
}

void ToggleFlag( const String& flagStr )
{
    GameFlag flag = StringToFlag( flagStr );
    ToggleFlag( flag );
}

void Config::LoadConfigFromBlackboard()
{

    // General
    LOAD_FROM_BLACKBOARD( appName );
    LOAD_FROM_BLACKBOARD( consoleHeightRatio );
    LOAD_FROM_BLACKBOARD( tileWidth );
    halfTileWidth = tileWidth / 2.f;
    LOAD_FROM_BLACKBOARD( testTexturePath );
    LOAD_FROM_BLACKBOARD( fontPath );
    LOAD_FROM_BLACKBOARD( fontSprtiesheetLayout );

    // Definitions
    LOAD_FROM_BLACKBOARD( tileDefPath );
    LOAD_FROM_BLACKBOARD( mapDefPath );
    LOAD_FROM_BLACKBOARD( actorDefPath );
    LOAD_FROM_BLACKBOARD( projectileDefPath );
    LOAD_FROM_BLACKBOARD( portalDefPath );
    LOAD_FROM_BLACKBOARD( adventureDefPath );
    LOAD_FROM_BLACKBOARD( itemDefPath );


    // Key bindings
    LOAD_FROM_BLACKBOARD( keyPause );
    LOAD_FROM_BLACKBOARD( keySlowTime );

    // Camera
    LOAD_FROM_BLACKBOARD( orthoSysSize );
    orthoSysWidth = orthoSysSize.x;
    orthoSysHeight = orthoSysSize.y;
    orthoSysBounds = AABB2( Vec2::ZEROS, orthoSysSize );
    clientAspect = orthoSysSize.x / orthoSysSize.y;
    LOAD_FROM_BLACKBOARD( cameraZoomTilesVertical );
    LOAD_FROM_BLACKBOARD( overlayColor );
    LOAD_FROM_BLACKBOARD( backgroundColor );

    // Time
    LOAD_FROM_BLACKBOARD( framsPerSecond );
    LOAD_FROM_BLACKBOARD( maxDeltaSeconds );
    LOAD_FROM_BLACKBOARD( speedOfTimeNormal );
    LOAD_FROM_BLACKBOARD( speedOfTimeSlow );
    LOAD_FROM_BLACKBOARD( speedOfTimeFast );

    // Dev mode
    LOAD_FROM_BLACKBOARD( numDevCircleSides );
    LOAD_FROM_BLACKBOARD( velocityColor );
    LOAD_FROM_BLACKBOARD( physicsRadiusColor );
    LOAD_FROM_BLACKBOARD( cosmeticRadiusColor );

    LOAD_FROM_BLACKBOARD( soundAttract );
    LOAD_FROM_BLACKBOARD( soundGameplay );
    LOAD_FROM_BLACKBOARD( soundPause );
    LOAD_FROM_BLACKBOARD( soundVictory );


}
