#pragma once
#include <vector>

#include "Engine/Math/AABB2.hpp"
class Image;
class GameMapChunk;

class GameMap
{
public:
	GameMap(){};
	~GameMap(){};

    void LoadFromImage( Image* image, const AABB2& extents, float minHeight,
                        float maxHeight, float chunkWidth );
    GameMapChunk* GetChunkAtIndex( const IVec2& idx );
    GameMapChunk* GetChunkAtPos( const Vec2& pos );
    AABB2 GetChunkExtentsAtIdx( const IVec2& idx );
    float GetHeight( const Vec2& pos );

private:

    AABB2 m_extents;
    float m_minHeight = 0;
    float m_maxHeight = 1;
    float m_chunkWidth = 10;

    Image* m_image = nullptr;
    std::vector<GameMapChunk*> m_chunks;


};
