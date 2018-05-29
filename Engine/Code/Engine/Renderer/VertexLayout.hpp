#pragma once
#include "Engine/Renderer/RendererEnums.hpp"
#include "Engine/Core/Types.hpp"

struct VertexBuilderData;

constexpr char* VERT_LAYOUT_END = "vert_layout_end";

struct VertexAttribute
{
    VertexAttribute() {};
    VertexAttribute( const char* name, RenderDataType dataType, uint count,
                     bool normalized, size_t offset )
        : m_name( name )
        , m_renderDataType( dataType )
        , m_elementCount( count )
        , m_isNormalized( normalized )
        , m_memberOffset( offset )
    {}

    VertexAttribute( const char* name ) : m_name( name ) {}
    static VertexAttribute END();

    String m_name; // e.g. POSITION
    RenderDataType m_renderDataType;
    uint m_elementCount;
    bool m_isNormalized;

    size_t m_memberOffset; // how far from start of element to this piece of data

};

//
typedef void( *VertCopierFunc )( void *dst, const VertexBuilderData* src, uint count );

class VertexLayout
{
public:
    VertexLayout( size_t stride, const VertexAttribute* layout,
                  VertCopierFunc copierFunc )
        : m_stride( stride )
        , m_attributes( layout, layout + GetAttribCountFromArray( layout ) )
        , Copier( copierFunc ) {}
    uint GetAttributeCount() const { return (uint) m_attributes.size(); };
    const VertexAttribute* GetAttribute( uint idx ) const { return &m_attributes[idx]; };
    bool HasAttribute( String name ) const;


public:
    std::vector<VertexAttribute> m_attributes;
    size_t m_stride; // how far between element

    VertCopierFunc Copier;
private:
    uint GetAttribCountFromArray( const VertexAttribute* layout );
};

