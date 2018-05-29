#pragma once
#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/Vec2.hpp"

class Texture;
class AABB2;
class Sprite;

class SpriteSheet
{
public:
    SpriteSheet( const Texture* texture, IVec2 layout );
    void SetTexture( const Texture* texture ) { m_spriteSheetTexture = texture; };
    AABB2 GetUVsForSpriteCoords( int x, int y ) const;
    AABB2 GetUVsForSpriteCoords( const IVec2& spriteCoords ) const; // for sprites
    AABB2 GetUVsForSpriteIndex( int spriteIndex ) const; // for sprite animations
    int GetNumSprites() const;
    const Texture* GetTexture() const { return m_spriteSheetTexture; }
    const IVec2 GetLayout() const { return m_spriteLayout; }

    Sprite* GetOrCreateSprite( const IVec2& coords, float height, const Vec2& pivot = Vec2::POINT_FIVES );

private:
    const Texture* 	m_spriteSheetTexture; 	// Texture w/grid-based layout of sprites
    const IVec2		m_spriteLayout;		// # of sprites across, and down, on the sheet

};

