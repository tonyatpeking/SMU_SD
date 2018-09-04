#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/FileIO/ObjLoader.hpp"

std::map<String, Mesh*> Mesh::s_loadedMeshes;

void Mesh::SetIndices( uint count, const uint* indices )
{
    m_indexBuffer.SetIndices( count, indices );
}

void Mesh::FromBuilder( const MeshBuilder& builder )
{
    m_vertexLayout = builder.m_vertexLayout;

    size_t vertArraySize = m_vertexLayout->m_stride * builder.GetVertCount();
    void *buffer = malloc( vertArraySize );
    m_vertexLayout->Copier( buffer, builder.m_verts.data(), builder.GetVertCount() );

    m_vertexBuffer.m_vertCount = builder.GetVertCount();
    m_vertexBuffer.m_vertStride = (uint) m_vertexLayout->m_stride;

    m_vertexBuffer.CopyToGPU( vertArraySize, buffer );
    free( buffer );

    SetIndices( builder.GetIndexCount(), builder.m_indices.data() );

    m_subMeshInstuct = builder.m_subMeshInstuct;
}

Mesh* Mesh::CreateOrGetMesh( const String& filePath, bool generateNormals,
                             bool generateTangents, bool useMikkT )
{
    if( !ContainerUtils::Contains( s_loadedMeshes, filePath ) )
    {
        MeshBuilder mb = ObjLoader::LoadFromFile( filePath.c_str() );

        if( generateNormals )
            mb.GenerateNormals();

        if( generateTangents )
        {
            if( useMikkT )
                mb.GenerateTangentsMikkT();
            else
                mb.GenerateTangentsFlat();
        }

        s_loadedMeshes[filePath] = mb.MakeMesh();
    }

    return s_loadedMeshes[filePath];
}

DrawInstruction& Mesh::GetSubMeshInstruction( uint subMeshId )
{
    return m_subMeshInstuct[subMeshId];
}



