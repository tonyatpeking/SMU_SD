#pragma once

#include <string>
#include "Engine/Renderer/SpriteAnimType.hpp"

class AABB2;
class Texture;
class SpriteAnimDefinition;
class SpriteSheet;

class SpriteAnim
{
public:
    SpriteAnim( const SpriteAnimDefinition* definition );

    void Update( float deltaSeconds );
    AABB2 GetCurrentUVs() const;	// Based on the current elapsed time
    void Pause() { m_isPlaying = false; };					// Starts unpaused (playing) by default
    void Resume() { m_isPlaying = true; };	    			// Resume after pausing
    void Reset();					// Rewinds to time 0 and starts (re)playing
    bool IsFinished() const { return m_isFinished; }
    bool IsPlaying() const { return m_isPlaying; }
    float GetSecondsElapsed() const { return m_elapsedSeconds; }
    float GetFractionElapsed() const;
    float GetSecondsRemaining() const;
    float GetFractionRemaining() const;
    void SetSecondsElapsed( float secondsElapsed ) { m_elapsedSeconds = secondsElapsed; };	    // Jump to specific time
    void SetFractionElapsed( float fractionElapsed );    // e.g. 0.33f for one-third in

    // Definition Accessors
    const SpriteAnimDefinition* Definition() const { return m_definition; };
    SpriteAnimType GetAnimType() const;
    const Texture* GetTexture() const;
    float GetDurationSeconds() const;
    float GetFPS() const;
    const SpriteSheet* GetSpriteSheet() const;

private:
    int GetCurrentSpriteIdx() const;

    const SpriteAnimDefinition* m_definition;

    bool m_isFinished = false;
    bool m_isPlaying = true;
    float m_elapsedSeconds = 0.f;
};
