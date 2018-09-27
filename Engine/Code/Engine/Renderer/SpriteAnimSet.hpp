#pragma once

#include <string>
#include <map>
#include "Engine/Renderer/SpriteAnimType.hpp"

class Texture;
class SpriteAnimSetDefinition;
class AABB2;
class SpriteAnim;

class SpriteAnimSet
{
public:
    SpriteAnimSet( SpriteAnimSetDefinition* animSetDef );
    ~SpriteAnimSet();

    void 			Update( float deltaSeconds );
    void 			StartAnim( SpriteAnimType animType );
    void 			SetCurrentAnim( SpriteAnimType animType );
    const Texture* 	GetCurrentTexture() const;
    AABB2 			GetCurrentUVs() const;
    SpriteAnim*     GetCurrentAnim() { return m_currentAnim; };
    bool            HasAnim( SpriteAnimType animType );

protected:
    SpriteAnimSetDefinition*			    m_animSetDef = nullptr;
    map< SpriteAnimType, SpriteAnim* >	m_anims;
    SpriteAnim*					            m_currentAnim = nullptr;
};
