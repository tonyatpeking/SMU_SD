#pragma once

#include <map>
#include <string>

#include "Engine/Renderer/SpriteAnimType.hpp"

class Renderer;
class XMLElement;
class SpriteAnimDefinition;

class SpriteAnimSetDefinition
{
    friend class SpriteAnimSet;

public:
    SpriteAnimSetDefinition( const XMLElement& xmlAnimSetDef, Renderer& renderer );
    ~SpriteAnimSetDefinition();
    // swaps the sprite sheets in this def for another one, preserves the layout
    void SwapSpriteSheet( const String& spriteSheetPath, Renderer& renderer );

protected:
    void AddAnimDef( SpriteAnimDefinition* animDef );
    std::map< SpriteAnimType, SpriteAnimDefinition* > m_animDefs;
    SpriteAnimType m_defaultAnimType = SpriteAnimType::IDLE;

};
