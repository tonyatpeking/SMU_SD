#pragma once
#include "Engine/Math/IVec2.hpp"
#include "Engine/Core/GameObject.hpp"

class GameMap;
class Renderable;

class GameMapChunk : public GameObject
{
public:
    GameMapChunk( GameMap* map, IVec2 index );
    ~GameMapChunk();
    void GenerateRenderable();


private:
    GameMap * m_map = nullptr;
    IVec2 m_index;

};
