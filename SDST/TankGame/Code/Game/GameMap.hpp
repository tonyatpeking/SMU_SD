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

    GameMapChunk* GetChunkAtCoord( const IVec2& coord );
    GameMapChunk* GetChunkAtPos( const Vec2& pos );

    // Can return index that is out of range
    IVec2 PosToCoord( const Vec2& pos );


    bool IsCoordOnMap( const IVec2& coord );

    AABB2 GetChunkExtentsAtCoord( const IVec2& coord );

    float GetHeightAtPos( const Vec2& pos );
    Vec3 GetNormalAtPos( const Vec2& pos );

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
