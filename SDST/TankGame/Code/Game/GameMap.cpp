#include "Engine/Core/Image.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Math/SurfacePatch.hpp"
#include "Engine/Math/Raycast.hpp"
#include "Engine/Math/GridStepper2D.hpp"
#include "Engine/Math/Ray3.hpp"

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
    m_bounds = AABB3( Vec3( m_extents.GetCenter() ),
                      m_extents.GetWidth(),
                      maxHeight - m_minHeight,
                      m_extents.GetHeight() );
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

GameMapChunk* GameMap::GetChunkAtCoord( const IVec2& coord )
{
    if( !IsCoordOnMap( coord ) )
        return nullptr;

    uint idx = IVec2::CoordsToIndex( coord, m_chunkCounts.x );
    return m_chunks[idx];
}

GameMapChunk* GameMap::GetChunkAtPos( const Vec2& pos )
{
    IVec2 idx = PosToCoord( pos );

    return GetChunkAtCoord( idx );
}

IVec2 GameMap::PosToCoord( const Vec2& pos )
{
    Vec2 relativeToMins = pos - m_extents.mins;
    Vec2 idxFloat = relativeToMins / m_chunkSize;
    return IVec2::Floor( idxFloat );
}

bool GameMap::IsCoordOnMap( const IVec2& coord )
{
    return coord.x >= 0
        && coord.y >= 0
        && coord.x < m_chunkCounts.x
        && coord.y < m_chunkCounts.y;
}

AABB2 GameMap::GetChunkExtentsAtCoord( const IVec2& coord )
{
    AABB2 chunkExtents;
    chunkExtents.mins = m_extents.mins + ( m_chunkSize * Vec2( coord ) );
    chunkExtents.maxs = chunkExtents.mins + m_chunkSize;
    return chunkExtents;
}

float GameMap::GetHeightAtPos( const Vec2& pos )
{
    if( !m_heightMap || !m_surface )
        return m_minHeight;

    if( !m_extents.IsPointInside( pos ) )
        return m_minHeight;

    Vec2 uv = m_extents.GetNormalizedPosition( pos );

    float height = m_surface->EvalPosition( uv ).y;

    return height;
}

Vec3 GameMap::GetNormalAtPos( const Vec2& pos )
{
    if( !m_heightMap || !m_surface )
        return Vec3::UP;

    if( !m_extents.IsPointInside( pos ) )
        return Vec3::UP;

    Vec2 uv = m_extents.GetNormalizedPosition( pos );

    return m_surface->EvalNormal( uv );


}

RaycastHit3 GameMap::RaycastMap( const Ray3& ray, float maxDist )
{
    if( maxDist == -1 )
    {
        maxDist = m_bounds.GetDiagonal3D();
    }

    Vec2 gridCellSize = GetGridCellLength();

    GridStepper2D stepper = GridStepper2D( Ray2::FromRay3XZ( ray ), gridCellSize );

    RaycastHit3 hit{};
    float prevT = 0;
    while( stepper.GetCurrentT() < maxDist )
    {
        prevT = stepper.GetCurrentT();
        stepper.Step();
        Vec3 rayPos = ray.Evaluate( stepper.GetCurrentT() );
        float mapHeight = GetHeightAtPos( Vec2::MakeFromXZ( rayPos ) );
        if( rayPos.y < mapHeight )
        {

        }

    }

    return hit;
}

Vec2 GameMap::GetGridCellLength()
{
    return m_chunkSize / Vec2( m_chunkCellsPerSide );
}

