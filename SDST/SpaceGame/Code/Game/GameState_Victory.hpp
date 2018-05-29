#pragma once
#include "Game/GameState.hpp"

class Menu;

class GameState_Victory : public GameState
{
public:
    GameState_Victory();
    ~GameState_Victory() override;
    void Update() override;
    void Render() const override;
    void OnEnter() override;
    void OnExit() override;
    void ProcessInput() override;
protected:

private:
};