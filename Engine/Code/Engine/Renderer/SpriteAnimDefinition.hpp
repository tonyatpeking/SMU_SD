#pragma once

#include <vector>
#include <string>

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Renderer/SpriteAnimType.hpp"
#include "Engine/Math/IRange.hpp"


class Texture;
class SpriteSheet;
class XMLElement;
class Renderer;

SMART_ENUM(
    SpriteAnimMode,

    INVALID,
    PLAY_TO_END,
    LOOPING
)

SpriteAnimMode StringToSpriteAnimMode( const String& spriteAnimModeStr );


class SpriteAnimDefinition
{
public:
    SpriteAnimDefinition( const XMLElement& xmlDef, Renderer& renderer );
    const Texture* GetTexture() const;

    // swaps the sprite sheets in this def for another one, preserves the layout
    void SwapSpriteSheet( const String& spriteSheetPath, Renderer& renderer );

    SpriteAnimType m_animType;
    SpriteAnimMode m_playbackMode = SpriteAnimMode::LOOPING;

    String m_spritePath;
    IVec2 m_spriteLayout;
    SpriteSheet* m_spriteSheet = nullptr;


    std::vector<int> m_spriteIndices;
    IRange m_spriteIndexRange = IRange( 0 );

    float m_duration = 0.f;
    float m_fps = 0.f;

    bool m_autoOrient = false;

private:
    void FillSpriteIndicesFromRange();
    void SetDurationFromFPS();
    void SetFPSFromDuration();
};

// This returns the spriteAnimMode from a string in xml
const SpriteAnimMode ParseXmlAttribute( const XMLElement& element, const char* attributeName, const SpriteAnimMode defaultValue );