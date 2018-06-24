#pragma once
#include <vector>

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
class Image;
class GameMapChunk;
class SurfacePatch;
struct RaycastHit3;
class Ray3;

class GameMap
{
    friend class GameMapChunk;
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

    Vec3 GetPos3D( const Vec2& pos );

    Vec2& GetChunkSize() { return m_chunkSize; };

    SurfacePatch* GetSurfacePatch() { return m_surface; };


    AABB2 GetMapExtents() { return m_extents; };

    // maxDist = -1 will use map 3d diagonal length
    RaycastHit3 RaycastMap( const Ray3& ray, float maxDist = -1 );

    // The grid length of the mesh in x or y
    Vec2 GetGridCellLength();

    float GetHeightRelativeToSurface( const Vec3& pos );


private:

    AABB2 m_extents;
    AABB3 m_bounds;
    float m_minHeight = 0;
    float m_maxHeight = 1;
    IVec2 m_chunkCounts;
    // In world units
    Vec2 m_chunkSize;
    IVec2 m_chunkCellsPerSide = IVec2(8,8);
    float m_uvRepeatPerChunk = 16;

    Image* m_heightMap = nullptr;
    std::vector<GameMapChunk*> m_chunks;
    SurfacePatch* m_surface = nullptr;

};
