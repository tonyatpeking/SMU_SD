#include "Engine/Core/Image.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Game/GameMap.hpp"


float GameMap::GetHeight( const Vec2& pos )
{
    if( !m_image )
        return 0;

    if( !m_extents.IsPointInside( pos ) )
        return 0;

    Vec2 uv = m_extents.GetNormalizedPosition( pos );
    float red = (float) m_image->GetTexelAtUV( uv ).r;
    float height = RangeMap( red, 0.f, 255.f, m_minHeight, m_maxHeight );
    return height;
}
