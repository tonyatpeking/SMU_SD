#pragma once
#include <vector>

#include "Engine/Math/AABB2.hpp"
class Image;
class GameMapChunk;
class SurfacePatch;

class GameMap
{
public:
    GameMap() {};
    ~GameMap();

    void LoadFromImage( Image* heightMap, const AABB2& extents, float minHeight,
                        float maxHeight, const IVec2& chunkCounts );

    GameMapChunk* GetChunkAtIndex( const IVec2& idx );
    GameMapChunk* GetChunkAtPos( const Vec2& pos );

    IVec2 PosToIndex( const Vec2& pos );

    // returns the mins of the chunk
    Vec2 IndexToPos( const IVec2& idx );

    AABB2 GetChunkExtentsAtIdx( const IVec2& idx );

    float GetHeight( const Vec2& pos );

    Vec2& GetChunkSize() { return m_chunkSize; };

    SurfacePatch* GetSurfacePatch() { return m_surface; };

    AABB2 GetMapExtents() { return m_extents; };

private:

    AABB2 m_extents;
    float m_minHeight = 0;
    float m_maxHeight = 1;
    IVec2 m_chunkCounts;
    // In world units
    Vec2 m_chunkSize;

    Image* m_heightMap = nullptr;
    std::vector<GameMapChunk*> m_chunks;
    SurfacePatch* m_surface = nullptr;

};
