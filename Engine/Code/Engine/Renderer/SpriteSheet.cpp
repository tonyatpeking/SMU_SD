#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"


SpriteSheet::SpriteSheet( const Texture* texture, IVec2 layout )
    : m_spriteSheetTexture( texture )
    , m_spriteLayout( layout )
{

}


AABB2 SpriteSheet::GetUVsForSpriteCoords( const IVec2& spriteCoords ) const
{
    float spriteWidth = 1.f / (float) m_spriteLayout.x;
    float spriteHeight = 1.f / (float) m_spriteLayout.y;
    Vec2 mins = Vec2( (float) spriteCoords.x * spriteWidth, (float) spriteCoords.y * spriteHeight );
    Vec2 maxs = mins + Vec2( spriteWidth, spriteHeight );
    return AABB2( mins, maxs );
}

AABB2 SpriteSheet::GetUVsForSpriteCoords( int x, int y ) const
{
    return GetUVsForSpriteCoords( IVec2( x, y ) );
}

AABB2 SpriteSheet::GetUVsForSpriteIndex( int spriteIndex ) const
{
    IVec2 coords = IVec2::IndexToCoords( spriteIndex, m_spriteLayout );
    return GetUVsForSpriteCoords( coords );
}

int SpriteSheet::GetNumSprites() const
{
    return IVec2::GetMaxIndex( m_spriteLayout );
}

