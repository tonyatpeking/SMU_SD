#pragma once
#include <vector>

#include "Engine/Core/SmartEnum.hpp"

#include "Game/GameStateType.hpp"
#include "Game/GameCommon.hpp"

class Map;
class Image;
class Entity;
class Player;
class Adventure;
class MainMenu;
class InventoryMenu;
class CubicSpline2D;
class GameObject;
class GameMap;
class Texture;
class GameState;
class Map;
typedef size_t PlaybackID;



class Game
{
public:
    Game();
    ~Game();

    void Update();
    void Render() const;
    void Initialize();

    void ChangeState( GameStateType gameStateType, float timeTillTransition = 0.f );


private:

    // developer options
    void ToggleSpeedUp();

    // loading
    void LoadConfigs();
    void LoadSounds();
    void LoadShaders();
    void LoadTextures();
    void DeleteTextures();

    void CheckSwapToNewState();

    void ToggleTests();

    void ProcessInput();
    void SetGameTimeScale( float timeSpeed );

    void SetIsPaused( bool paused );
    bool IsPaused() const;

    void StartGame();
    void EndGame();
    void ReloadDataAndRestartGame();

    void RenderPause() const;
    void MakePauseMenu();
    void UpdatePause();

    void ToggleDebugRenderModes();
public:
    int m_victoryTeam = -1;

    // Game resources, #TODO Move to res manager
    Image* m_noiseImage = nullptr;
    Texture* m_noiseTexture = nullptr;
    int m_noiseImageSize = 256;

    GameMap* m_map = nullptr;

    GameObject* m_shipHull;
    Texture* m_skybox = nullptr;


private:
    float m_pauseEffectCurrentTime = 0.f;
    float m_pauseEffectTotalTime = 0.5f;
    Menu * m_pauseMenu = nullptr;
    // State management
    GameState* m_currentGameState = nullptr;
    GameStateType m_transitioningToState = GameStateType::INVALID;
    float m_timeTillTransition = 0.f;

    float m_speedOfTime = g_config->speedOfTimeNormal;


    // UI
    MainMenu* m_mainMenu = nullptr;
    InventoryMenu* m_inventoryMenu = nullptr;

    PlaybackID m_overlayMusicPlaybackID;
    PlaybackID m_gameplayMusicPlaybackID;
};