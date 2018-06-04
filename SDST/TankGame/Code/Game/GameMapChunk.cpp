#include "Engine/Renderer/Renderable.hpp"

#include "Game/GameMapChunk.hpp"
#include "Game/GameMap.hpp"

GameMapChunk::GameMapChunk( GameMap* map, IVec2 index )
    : m_map( map )
    , m_index( index )
{
    GenerateRenderable();
}

GameMapChunk::~GameMapChunk()
{
    delete m_renderable;
}

void GameMapChunk::GenerateRenderable()
{
    m_renderable = new Renderable();
}
