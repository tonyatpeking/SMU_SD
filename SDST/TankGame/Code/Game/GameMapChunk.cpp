#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Math/SurfacePatch.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"

#include "Game/GameMapChunk.hpp"
#include "Game/GameMap.hpp"
#include "Game/GameCommon.hpp"

GameMapChunk::GameMapChunk( GameMap* map, IVec2 index )
    : m_map( map )
    , m_index( index )
{
    GenerateRenderable();
}

GameMapChunk::~GameMapChunk()
{
}

void GameMapChunk::GenerateRenderable()
{
    SetRenderable(new Renderable());

    AABB2 chunkExtents = m_map->GetChunkExtentsAtIdx( m_index );
    AABB2 mapExtents = m_map->GetMapExtents();

    AABB2 uv = mapExtents.GetNormalizedBounds( chunkExtents );

    SurfacePatch* surface = m_map->GetSurfacePatch();
    MeshBuilder mb = MeshBuilder::FromSurfacePatch( surface, 8, 8, uv, 16 );
    m_renderable->SetMesh( mb.MakeMesh() );

    Material* mat = new Material();
    mat->m_specularAmount = 0.1f;
    mat->m_specularPower = 1.f;
    mat->m_shaderPass = new ShaderPass();
    mat->m_shaderPass->m_program = ShaderProgram::CreateOrGetFromFiles(
        "Data/Shaders/lit" );
    mat->m_diffuse = g_renderer->CreateOrGetTexture( "Data/Images/Terrain/Grass_Diffuse.jpg" );
    mat->m_normal = g_renderer->CreateOrGetTexture( "Data/Images/Terrain/Grass_Normal.jpg" );
    m_renderable->SetMaterial( 0, mat );

}