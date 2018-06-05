#include "Engine/Core/Image.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Math/SurfacePatch.hpp"

#include "Game/GameMapChunk.hpp"
#include "Game/GameMap.hpp"


GameMap::~GameMap()
{
    ContainerUtils::DeletePointers( m_chunks );
    delete m_surface;
}

void GameMap::LoadFromImage( Image* heightMap, const AABB2& extents, float minHeight,
                             float maxHeight, const IVec2& chunkCounts )
{
    m_heightMap = heightMap;
    m_extents = extents;
    m_minHeight = minHeight;
    m_maxHeight = maxHeight;
    m_chunkCounts = chunkCounts;

    m_chunkSize = extents.GetDimensions() / (Vec2) chunkCounts; // assume non zero
    m_surface = new SurfacePatch_HeightMap( heightMap, extents, minHeight, maxHeight );
    for( int chunkIdxY = 0; chunkIdxY < chunkCounts.y; ++chunkIdxY )
    {
        for( int chunkIdxX = 0; chunkIdxX < chunkCounts.x; ++chunkIdxX )
        {
            GameMapChunk* chunk = new GameMapChunk( this, IVec2( chunkIdxX, chunkIdxY ) );
            chunk->GenerateRenderable();
            m_chunks.push_back( chunk );
        }
    }
}

AABB2 GameMap::GetChunkExtentsAtIdx( const IVec2& idx )
{
    AABB2 chunkExtents;
    chunkExtents.mins = m_extents.mins + ( m_chunkSize * Vec2( idx ) );
    chunkExtents.maxs = chunkExtents.mins + m_chunkSize;
    return chunkExtents;
}

float GameMap::GetHeight( const Vec2& pos )
{
    if( !m_heightMap || !m_surface )
        return m_minHeight;

    if( !m_extents.IsPointInside( pos ) )
        return m_minHeight;

    Vec2 uv = m_extents.GetNormalizedPosition( pos );

    float height = m_surface->EvalPosition( uv ).y;

    return height;
}
