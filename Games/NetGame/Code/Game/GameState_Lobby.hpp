#pragma once
#include "Game/GameState.hpp"

class GameState_Lobby : public GameState
{
public:
    GameState_Lobby();
    ~GameState_Lobby() override;
    void Update() override;
    void Render() const override;
    void OnEnter() override;
    void OnExit() override;
    void ProcessInput() override;
protected:

};