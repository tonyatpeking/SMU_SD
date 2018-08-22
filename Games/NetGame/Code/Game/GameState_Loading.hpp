#pragma once
#include "Game/GameState.hpp"

class GameState_Loading : public GameState
{
public:
    GameState_Loading();
    ~GameState_Loading() override;
    void Update() override;
    void Render() const override;
    void OnEnter() override;
    void OnExit() override;
    void ProcessInput() override;
protected:
    // Load on second frame so that loading frame is rendered
    void LoadResourcesOnSecondFrame();
    void MakeSkyBox();
};