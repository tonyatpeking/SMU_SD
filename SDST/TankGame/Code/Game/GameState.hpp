#pragma once
#include <string>
#include "Game/GameStateType.hpp"

class TweenSystem;

class GameState
{
public:
    static GameState* MakeGameState( GameStateType gameStateType );
    virtual ~GameState();
    virtual void Update();
    virtual void Render() const;
    virtual void OnEnter();
    virtual void OnExit();
    virtual void ProcessInput();
    GameStateType GetType() { return m_gameStateType; };
    String ToString();
protected:
    GameState( GameStateType gameStateType );
    float m_UITimeOfEnter = 0.f;
    GameStateType m_gameStateType = GameStateType::INVALID;
    TweenSystem* m_tweenSystem; // owns
};