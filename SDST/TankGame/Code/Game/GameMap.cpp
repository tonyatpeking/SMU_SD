#include <functional>

#include "Engine/Core/Image.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Math/SurfacePatch.hpp"
#include "Engine/Math/Raycast.hpp"
#include "Engine/Math/GridStepper2D.hpp"
#include "Engine/Math/Ray3.hpp"
#include "Engine/Math/Solver.hpp"
#include "Engine/Renderer/DebugRender.hpp"

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
            if( chunkIdxX == 10 && chunkIdxY == 10 )
                continue;
            if( chunkIdxX == 8 && chunkIdxY == 8 )
                continue;
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

Vec3 GameMap::GetPos3D( const Vec2& pos )
{
    float height = GetHeightAtPos( pos );
    return Vec3::MakeFromXZ( pos, height );
}

RaycastHit3 GameMap::RaycastMap( const Ray3& ray, float maxDist )
{
    if( maxDist == -1 )
    {
        maxDist = m_bounds.GetDiagonal3D();
    }

    Vec2 gridCellSize = GetGridCellLength();

    Ray2 ray2 = Ray2::FromRay3XZ( ray );
    GridStepper2D stepper = GridStepper2D( ray2, gridCellSize );

    RaycastHit3 hit{};
    float prevT = 0;
    float currentT = 0;
    while( currentT < maxDist )
    {
        // Use Grid stepper to narrow down search
        prevT = currentT;
        stepper.Step();
        currentT = stepper.GetCurrentT();
        Vec3 rayPos = ray.Evaluate( currentT );

        float relHeight = GetHeightRelativeToSurface( rayPos );

        if( relHeight < 0.f )
        {
            // Use Binary Search to find intersection
            std::function<float( float t )> eval = [&]( float t ) -> float {
                Vec3 pos = ray.Evaluate( t );
                return GetHeightRelativeToSurface( pos );
            };
            float solveT = Solver::BinarySearch( prevT, currentT, eval, 10 );
            hit.m_distance = solveT;
            hit.m_hit = true;
            Vec2 pos2D = ray2.Evaluate( solveT );
            hit.m_normal = GetNormalAtPos( pos2D );
            hit.m_position = GetPos3D( pos2D );
            break;
        }
    }
    return hit;
}

Vec2 GameMap::GetGridCellLength()
{
    return m_chunkSize / Vec2( m_chunkCellsPerSide );
}

float GameMap::GetHeightRelativeToSurface( const Vec3& pos )
{
    float mapHeight = GetHeightAtPos( Vec2::MakeFromXZ( pos ) );
    return pos.y - mapHeight;
}

