#pragma once
#include "Game/GameState.hpp"

class Menu;
class TweenSystem;

class GameState_MainMenu : public GameState
{
public:
    GameState_MainMenu();
    ~GameState_MainMenu() override;
    void Update() override;
    void Render() const override;
    void OnEnter() override;
    void OnExit() override;
    void ProcessInput() override;

private:
    void StartGame();
    Menu* m_menu;
    float m_textOffset = -200;
    size_t m_audioPlaybackID = -1;
};