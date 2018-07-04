#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/RendererEnums.hpp"
#include "Engine/Renderer/DrawInstruction.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/VertexLayout.hpp"
#include "Engine/Renderer/Vertex.hpp"

class Mesh;
class SurfacePatch;
class Mat4;

typedef std::vector<VertexBuilderData> VertexDataVec;

class MeshBuilder
{
public:
    MeshBuilder() {};
    virtual ~MeshBuilder() {};

    // uvTile only affects the texture uv
    static MeshBuilder FromSurfacePatch(
        SurfacePatch* sp, int uGridLines = 20,
        int vGridLines = 20, const AABB2& uvBounds = AABB2::ZEROS_ONES,
        float uvTile = 1.f );

    void BeginSubMesh( DrawPrimitive prim = DrawPrimitive::TRIANGLES,
                       bool useIndices = true );
    void EndSubMesh();

    Mesh* MakeMesh();

    template<typename T>
    void SetVertexType() { m_vertexLayout = &T::s_vertexLayout; }


    void SetColor( const Rgba& color );
    void SetUV( const Vec2& uv );
    void SetNormal( const Vec3& normal );
    void SetTangent( const Vec3& tangent, bool flipBitangent = false );

    void PushPos( const Vec3& pos );

    // The following assume DrawPrimitive::TRIANGLES
    void AddFace( const Vec3& p0, const Vec3& p1, const Vec3& p2 );
    void AddFace( const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3 );
    void AddFaceIdx( uint idx0, uint idx1, uint idx2 );
    void AddFaceIdx( uint idx0, uint idx1, uint idx2, uint idx3 );
    void AddFaceIdxRange( uint startIdx, uint endIdx );
    void AddBuilder( const MeshBuilder& builderToAdd );
    void AddQuad2D( const AABB2 bounds = AABB2::NEG_ONES_ONES,
                    const Rgba& tint = Rgba::WHITE,
                    const AABB2& uvs = AABB2::ZEROS_ONES );

    void AddQuad( const Vec3& pos, const Vec3& right, const Vec3& up,
                  const AABB2& bounds = AABB2::NEG_ONES_ONES,
                  const Rgba& tint = Rgba::WHITE,
                  const AABB2& uvs = AABB2::ZEROS_ONES );

    void GenerateNormals();
    void GenerateTangentsMikkT();
    // Tangents will not be aligned with UV, only for flat lighting
    void GenerateTangentsFlat();

    void ReserveIndices( size_t idxCount );
    void ReserveVertices( size_t vertCount );
    void Reserve( size_t vertCount, size_t idxCount );

    void Clear();

    VertexBuilderData& GetVertex( uint vertexIdx );
    const VertexBuilderData& GetVertex( uint vertexIdx ) const;
    uint GetIndex( uint indexIdx );
    uint GetVertCount() const;
    uint GetIndexCount() const;

    void TransformAllVerts( const Mat4& transform );

    Uints m_indices;
    VertexDataVec m_verts;
    const VertexLayout* m_vertexLayout = &VertexLit::s_vertexLayout;
    std::vector<DrawInstruction> m_subMeshInstuct;

    void CalculateBounds();
    AABB3 GetLocalBounds() const { return m_localBounds; };

private:
    VertexBuilderData m_currentVert;
    AABB3 m_localBounds;

};
