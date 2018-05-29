#include "Engine/Renderer/Vertex.hpp"
#include "Engine/Renderer/RendererEnums.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

//--------------------------------------------------------------------------------------
// VertexPCU
//--------------------------------------------------------------------------------------
const VertexAttribute VertexPCU::s_attributes[] ={
    VertexAttribute( "POSITION", RenderDataType::FLOAT,         3, false, offsetof( VertexPCU, m_position ) ),
    VertexAttribute( "COLOR",    RenderDataType::UNSIGNED_BYTE, 4, true,  offsetof( VertexPCU, m_color ) ),
    VertexAttribute( "UV",       RenderDataType::FLOAT,         2, false, offsetof( VertexPCU, m_uv ) ),

    VertexAttribute::END()
};

void VertexPCU::s_copier( void *dst, const VertexBuilderData* src, uint count )
{
    VertexPCU *dest = (VertexPCU*) dst;
    for (uint idx = 0; idx < count ; ++idx)
    {
        dest[idx].m_position = src[idx].m_position;
        dest[idx].m_color = src[idx].m_color;
        dest[idx].m_uv = src[idx].m_uv;
    }
}

const VertexLayout VertexPCU::s_vertexLayout = VertexLayout(
    sizeof( VertexPCU ), VertexPCU::s_attributes, VertexPCU::s_copier );


//--------------------------------------------------------------------------------------
// VertexLit
//--------------------------------------------------------------------------------------

const VertexAttribute VertexLit::s_attributes[] ={
    VertexAttribute( "POSITION", RenderDataType::FLOAT,         3, false, offsetof( VertexLit, m_position ) ),
    VertexAttribute( "COLOR",    RenderDataType::UNSIGNED_BYTE, 4, true,  offsetof( VertexLit, m_color ) ),
    VertexAttribute( "UV",       RenderDataType::FLOAT,         2, false, offsetof( VertexLit, m_uv ) ),
    VertexAttribute( "NORMAL",   RenderDataType::FLOAT,         3, false, offsetof( VertexLit, m_normal ) ),
    VertexAttribute( "TANGENT",  RenderDataType::FLOAT,         4, false, offsetof( VertexLit, m_tangent ) ),

    VertexAttribute::END()
};

void VertexLit::s_copier( void *dst, const VertexBuilderData* src, uint count )
{
    VertexLit *dest = (VertexLit*) dst;
    for( uint idx = 0; idx < count; ++idx )
    {
        dest[idx].m_position = src[idx].m_position;
        dest[idx].m_color = src[idx].m_color;
        dest[idx].m_uv = src[idx].m_uv;
        dest[idx].m_normal = src[idx].m_normal;
        dest[idx].m_tangent = src[idx].m_tangent;
    }
}

const VertexLayout VertexLit::s_vertexLayout = VertexLayout(
    sizeof( VertexLit ), VertexLit::s_attributes, VertexLit::s_copier );