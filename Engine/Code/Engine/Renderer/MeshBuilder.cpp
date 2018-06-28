#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Math/SurfacePatch.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Math/Mat4.hpp"

#include "ThirdParty/mikktspace/mikktspace.h"


MeshBuilder MeshBuilder::FromSurfacePatch( SurfacePatch* sp,
                                           int uGridCells, int vGridCells,
                                           const AABB2& uvBounds, float uvTile )
{
    MeshBuilder mb{};
    mb.BeginSubMesh();

    float uRange = uvBounds.GetWidth();
    if( uRange == 0 )
        uRange = 0.0001f;
    float vRange = uvBounds.GetHeight();
    if( vRange == 0 )
        vRange = 0.0001f;

    float uSpacing = 1.f / ( (float) uGridCells );
    uSpacing = uSpacing * uRange;

    float vSpacing = 1.f / ( (float) vGridCells );
    vSpacing = vSpacing * vRange;

    // Set all the vertices
    for( int vIdx = 0; vIdx < vGridCells + 1; ++vIdx )
    {
        float v = uvBounds.mins.v + ( vIdx * vSpacing );
        for( int uIdx = 0; uIdx < uGridCells + 1; ++uIdx )
        {
            float u = uvBounds.mins.u + ( uIdx * uSpacing );
            Vec2 uv = Vec2( u, v );
            mb.SetUV( uv * uvTile );
            Vec3 tangent = sp->EvalTangent( uv );
            Vec3 bitangent = sp->EvalBitangent( uv );
            Vec3 normal = sp->CalcNormal( bitangent, tangent );
            mb.SetNormal( normal );
            mb.SetTangent( tangent );

            Vec3 position = sp->EvalPosition( uv );
            mb.PushPos( position );
        }
    }
    // Set all the indices
    for( int vIdx = 0; vIdx < vGridCells; ++vIdx )
    {
        for( int uIdx = 0; uIdx < uGridCells; ++uIdx )
        {
            // idx3 idx2
            // idx0 idx1
            int idx0 = IVec2::CoordsToIndex( uIdx, vIdx, uGridCells + 1);
            int idx1 = IVec2::CoordsToIndex( uIdx + 1, vIdx, uGridCells + 1 );
            int idx2 = IVec2::CoordsToIndex( uIdx + 1, vIdx + 1, uGridCells + 1 );
            int idx3 = IVec2::CoordsToIndex( uIdx, vIdx + 1, uGridCells + 1 );
            mb.AddFaceIdx( idx0, idx1, idx2, idx3 );
        }
    }

    mb.EndSubMesh();
    return mb;
}

void MeshBuilder::BeginSubMesh( DrawPrimitive prim, bool useIndices )
{
    DrawInstruction instruct{};
    instruct.m_drawPrimitive = prim;
    instruct.m_useIndices = useIndices;

    if( useIndices )
        instruct.m_startIdx = (uint) m_indices.size();
    else
        instruct.m_startIdx = (uint) m_verts.size();

    m_subMeshInstuct.push_back( instruct );

}

void MeshBuilder::EndSubMesh()
{
    DrawInstruction& instruct = m_subMeshInstuct.back();
    uint endIdx;

    if( instruct.m_useIndices )
        endIdx = (uint) m_indices.size();
    else
        endIdx = (uint) m_verts.size();

    instruct.m_elemCount = endIdx - instruct.m_startIdx;
}

void MeshBuilder::SetColor( const Rgba& color )
{
    m_currentVert.m_color = color;
}

void MeshBuilder::SetUV( const Vec2& uv )
{
    m_currentVert.m_uv = uv;
}

void MeshBuilder::SetNormal( const Vec3& normal )
{
    m_currentVert.m_normal = normal;
}

void MeshBuilder::SetTangent( const Vec3& tangent, bool flipBitangent /*= false */ )
{
    float flip = flipBitangent ? -1.f : 1.f;
    m_currentVert.m_tangent = Vec4( tangent, flip );
}

void MeshBuilder::PushPos( const Vec3& pos )
{
    m_currentVert.m_position = pos;
    m_verts.push_back( m_currentVert );
}

void MeshBuilder::AddFaceIdx( uint idx0, uint idx1, uint idx2 )
{
    m_indices.push_back( idx0 );
    m_indices.push_back( idx1 );
    m_indices.push_back( idx2 );
}

void MeshBuilder::AddFaceIdx( uint idx0, uint idx1, uint idx2, uint idx3 )
{
    AddFaceIdx( idx0, idx1, idx2 );
    AddFaceIdx( idx0, idx2, idx3 );
}

void MeshBuilder::AddFaceIdxRange( uint startIdx, uint endIdx )
{
    uint numVerts = endIdx - startIdx + 1;
    if( startIdx >= endIdx || numVerts < 3 )
    {
        LOG_WARNING( "Invalid start and end Idx for AddFaceIdxRange" );
        return;
    }

    for( uint secondIdx = startIdx + 1; secondIdx < endIdx; ++secondIdx )
        AddFaceIdx( startIdx, secondIdx, secondIdx + 1 );
}

void MeshBuilder::AddBuilder( const MeshBuilder& builderToAdd )
{
    uint startVertCount = GetVertCount();
    uint addVertCount = builderToAdd.GetVertCount();
    ReserveVertices( startVertCount + addVertCount );
    for( uint vertIdx = 0; vertIdx < addVertCount; ++vertIdx )
    {
        m_verts.push_back( builderToAdd.m_verts[vertIdx] );
    }

    uint startIdxCount = GetIndexCount();
    uint addIdxCount = builderToAdd.GetIndexCount();
    ReserveIndices( startIdxCount + addIdxCount );
    for( uint idxIdx = 0; idxIdx < addIdxCount; ++idxIdx )
    {
        m_indices.push_back( builderToAdd.m_indices[idxIdx] + startVertCount );
    }
}

void MeshBuilder::AddQuad2D( const AABB2 bounds, const Rgba& tint, const AABB2& uvs )
{
    // vert indices
    // 3 2
    // 0 1

    SetNormal( Vec3::BACKWARD );
    SetTangent( Vec3::RIGHT );
    SetColor( tint );

    SetUV( uvs.mins );
    PushPos( (Vec3) bounds.mins );

    SetUV( uvs.GetMaxXMinY() );
    PushPos( (Vec3) bounds.GetMaxXMinY() );

    SetUV( uvs.maxs );
    PushPos( (Vec3) bounds.maxs );

    SetUV( uvs.GetMinXMaxY() );
    PushPos( (Vec3) bounds.GetMinXMaxY() );

    int lastVert = GetVertCount() - 1;

    AddFaceIdx( lastVert - 3,
                lastVert - 2,
                lastVert - 1,
                lastVert );
}

void MeshBuilder::AddQuad( const Vec3& pos, const Vec3& right, const Vec3& up,
                           const AABB2& bounds, const Rgba& tint, const AABB2& uvs )
{
    Vec3 normal = Cross( up, right );
    SetNormal( normal );
    SetColor( tint );
    Vec3 rightScaled = right * bounds.GetWidth();
    Vec3 upScaled = up * bounds.GetHeight();
    // 3 2
    // 0 1
    Vec3 p0 = pos - ( rightScaled * 0.5f ) - ( upScaled * 0.5f );
    Vec3 p1 = p0 + rightScaled;
    Vec3 p2 = p1 + upScaled;
    Vec3 p3 = p0 + upScaled;

    SetUV( uvs.mins );
    PushPos( p0 );

    SetUV( uvs.GetMaxXMinY() );
    PushPos( p1 );

    SetUV( uvs.maxs );
    PushPos( p2 );

    SetUV( uvs.GetMinXMaxY() );
    PushPos( p3 );

    int lastVert = GetVertCount() - 1;
    AddFaceIdx( lastVert - 3,
                lastVert - 2,
                lastVert - 1,
                lastVert );
}

void MeshBuilder::GenerateNormals()
{
    // Each vertex may me used by many faces, so the normals for each face is generated
    // first and then the average is used for the vertex

    // all the face normals of one vertex
    typedef std::vector<Vec3> Normals;

    // the Normals of all the vertices
    typedef std::vector<Normals> VertexNormals;
    VertexNormals vertexNormals;
    vertexNormals.resize( m_verts.size() );
    uint indexCount = (uint) m_indices.size();
    if( indexCount % 3 != 0 )
    {
        LOG_WARNING( "index count not multiple of 3, cannot GenerateNormals!" );
        return;
    }

    // loop through all faces and store normals
    for( uint idx = 0; idx < indexCount; idx += 3 )
    {
        uint idx0 = m_indices[idx];
        uint idx1 = m_indices[idx + 1];
        uint idx2 = m_indices[idx + 2];
        Vec3 pos0 = m_verts[idx0].m_position;
        Vec3 pos1 = m_verts[idx1].m_position;
        Vec3 pos2 = m_verts[idx2].m_position;
        Vec3 normal = Cross( pos2 - pos0, pos1 - pos0 );
        normal.NormalizeAndGetLength();
        vertexNormals[idx0].push_back( normal );
        vertexNormals[idx1].push_back( normal );
        vertexNormals[idx2].push_back( normal );
    }

    // loop through all normals, average and assign to verts
    for( uint vertIdx = 0; vertIdx < m_verts.size(); ++vertIdx )
    {
        Normals& normals = vertexNormals[vertIdx];
        size_t normalsCount = normals.size();
        if( normalsCount == 0 )
            continue;
        Vec3 sumNormal = ContainerUtils::Sum( normals );
        Vec3 averageNormal = sumNormal / (float) normalsCount;
        averageNormal.NormalizeAndGetLength();
        m_verts[vertIdx].m_normal = averageNormal;

    }
}

Mesh* MeshBuilder::MakeMesh()
{
    CalculateBounds();
    Mesh *mesh = new Mesh();
    mesh->FromBuilder( *this );
    mesh->SetBounds( m_localBounds );
    return mesh;
}

void MeshBuilder::ReserveIndices( size_t idxCount )
{
    m_indices.reserve( idxCount );
}

void MeshBuilder::ReserveVertices( size_t vertCount )
{
    m_verts.reserve( vertCount );
}

void MeshBuilder::Reserve( size_t vertCount, size_t idxCount )
{
    ReserveVertices( vertCount );
    ReserveIndices( idxCount );
}

void MeshBuilder::Clear()
{
    m_verts.clear();
    m_indices.clear();
    m_subMeshInstuct.clear();
    m_currentVert = VertexBuilderData{};
}

uint MeshBuilder::GetVertCount() const
{
    return (uint) m_verts.size();
}

uint MeshBuilder::GetIndexCount() const
{
    return (uint) m_indices.size();
}


void MeshBuilder::TransformAllVerts( const Mat4& transform )
{
    if( m_vertexLayout->HasAttribute( "POSITION" ) )
    {
        for( auto& vert : m_verts )
            vert.m_position = Vec3( transform * Vec4( vert.m_position, 1 ) );
    }

    if( m_vertexLayout->HasAttribute( "NORMAL" ) )
    {
        for( auto& vert : m_verts )
            vert.m_normal = Vec3( transform * Vec4( vert.m_normal, 0 ) );
    }

    if( m_vertexLayout->HasAttribute( "TANGENT" ) )
    {
        for( auto& vert : m_verts )
        {
            float sign = vert.m_tangent.w;
            Vec4 tangent = vert.m_tangent;
            tangent.w = 0;
            tangent = transform * tangent;
            tangent.w = sign;
            vert.m_tangent = tangent;
        }
    }
}

void MeshBuilder::CalculateBounds()
{
    for (int i = 0; i < m_verts.size() ; ++i)
    {
        m_localBounds.StretchToIncludePoint( m_verts[i].m_position );
    }
}

VertexBuilderData& MeshBuilder::GetVertex( uint vertexIdx )
{
    return m_verts[vertexIdx];
}

const VertexBuilderData& MeshBuilder::GetVertex( uint vertexIdx ) const
{
    return m_verts[vertexIdx];
}

uint MeshBuilder::GetIndex( uint indexIdx )
{
    return m_indices[indexIdx];
}

//--------------------------------------------------------------------------------------
// Mikkt tangent generation
//--------------------------------------------------------------------------------------
namespace
{
MeshBuilder* GetBuilderFromContext( const SMikkTSpaceContext* pContext )
{
    return (MeshBuilder*) pContext->m_pUserData;
}

VertexBuilderData& GetVertexFromContext( const SMikkTSpaceContext* pContext,
                                         int iFace, int iVert )
{
    MeshBuilder* builder = GetBuilderFromContext( pContext );
    uint idx = builder->GetIndex( iFace * 3 + iVert );
    return builder->GetVertex( idx );
}


int GetNumFaces( const SMikkTSpaceContext* pContext )
{
    MeshBuilder* builder = GetBuilderFromContext( pContext );
    return builder->GetIndexCount() / 3;
}

int GetNumVerticesOfFace( const SMikkTSpaceContext* pContext, const int iFace )
{
    UNUSED( pContext );
    UNUSED( iFace );
    return 3;
}

void GetPosition( const SMikkTSpaceContext * pContext, float fvPosOut[],
                  const int iFace, const int iVert )
{
    VertexBuilderData& vertex = GetVertexFromContext( pContext, iFace, iVert );
    fvPosOut[0] = vertex.m_position.x;
    fvPosOut[1] = vertex.m_position.y;
    fvPosOut[2] = vertex.m_position.z;
}

void GetNormal( const SMikkTSpaceContext * pContext, float fvNormOut[],
                const int iFace, const int iVert )
{
    VertexBuilderData& vertex = GetVertexFromContext( pContext, iFace, iVert );
    fvNormOut[0] = vertex.m_normal.x;
    fvNormOut[1] = vertex.m_normal.y;
    fvNormOut[2] = vertex.m_normal.z;
}

void GetTexCoord( const SMikkTSpaceContext * pContext, float fvTexcOut[],
                  const int iFace, const int iVert )
{
    VertexBuilderData& vertex = GetVertexFromContext( pContext, iFace, iVert );
    fvTexcOut[0] = vertex.m_uv.x;
    fvTexcOut[1] = vertex.m_uv.y;

}

void SetTSpaceBasic( const SMikkTSpaceContext * pContext, const float fvTangent[],
                     const float fSign, const int iFace, const int iVert )
{
    VertexBuilderData& vertex = GetVertexFromContext( pContext, iFace, iVert );
    vertex.m_tangent.x = fvTangent[0];
    vertex.m_tangent.y = fvTangent[1];
    vertex.m_tangent.z = fvTangent[2];
    vertex.m_tangent.w = fSign;
}

}

void MeshBuilder::GenerateTangentsMikkT()
{
    // Setup interface
    SMikkTSpaceInterface mtInterface{};
    mtInterface.m_getNumFaces = GetNumFaces;
    mtInterface.m_getNumVerticesOfFace = GetNumVerticesOfFace;
    mtInterface.m_getPosition = GetPosition;
    mtInterface.m_getNormal = GetNormal;
    mtInterface.m_getTexCoord = GetTexCoord;
    mtInterface.m_setTSpaceBasic = SetTSpaceBasic;

    SMikkTSpaceContext mtContext{};
    mtContext.m_pInterface = &mtInterface;
    mtContext.m_pUserData = this;

    genTangSpaceDefault( &mtContext );
}

void MeshBuilder::GenerateTangentsFlat()
{
    for( uint vertIdx = 0; vertIdx < m_verts.size(); ++vertIdx )
    {
        Vec3 normal = m_verts[vertIdx].m_normal;
        //calculate tangent
        Vec3 tangent = Cross( Vec3::UP, normal );
        if( tangent == Vec3::ZEROS )
            tangent = Cross( Vec3::RIGHT, normal );
        tangent.NormalizeAndGetLength();
        m_verts[vertIdx].m_tangent = Vec4( tangent, 1.f );
    }
}
