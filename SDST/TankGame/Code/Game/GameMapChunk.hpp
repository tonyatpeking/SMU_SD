#pragma once
#include "Engine/Math/IVec2.hpp"

class GameMap;
class Renderable;

class GameMapChunk
{
public:
    GameMapChunk( GameMap* map, IVec2 index );
    ~GameMapChunk();
    void GenerateRenderable();


private:
    GameMap * m_map = nullptr;
    IVec2 m_index;
    Renderable* m_renderable = nullptr;
};
