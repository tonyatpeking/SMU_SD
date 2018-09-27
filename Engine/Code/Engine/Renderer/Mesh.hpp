#pragma once
#include <vector>
#include "Engine/Core/Types.hpp"
#include "Engine/Renderer/Vertex.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/GameObject/Transform.hpp"
#include "Engine/Renderer/RendererEnums.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/DrawInstruction.hpp"
#include "Engine/Math/AABB3.hpp"

class RenderBuffer;
class Material;
class Camera;
class VertexLayout;
class MeshBuilder;

class Mesh
{
public:
    Mesh() {};
    virtual ~Mesh() {};
    Mesh( const Mesh& mesh ) = delete;
    void operator = ( const Mesh& mesh ) = delete;

    void SetIndices( uint count, const uint* indices );

    template<typename T>
    void SetVertices( uint count, const T* verts )
    {
        m_vertexBuffer.SetVertices( count, verts );
    }

    void FromBuilder( const MeshBuilder& builder );

    static Mesh* CreateOrGetMesh( const string& filePath, bool generateNormals = false,
                                  bool generateTangents = false, bool useMikkT = true );

    DrawInstruction& GetSubMeshInstruction( uint subMeshId );
    uint GetSubMeshCount() { return (uint) m_subMeshInstuct.size(); };

    AABB3 GetLocalBounds() const { return m_localBounds; };
    void SetLocalBounds( AABB3& bounds ) { m_localBounds = bounds; };
public:
    static map<string, Mesh*> s_loadedMeshes;

    VertexBuffer m_vertexBuffer;
    IndexBuffer m_indexBuffer;
    vector<DrawInstruction> m_subMeshInstuct;
    const VertexLayout* m_vertexLayout = nullptr;
    AABB3 m_localBounds;
};