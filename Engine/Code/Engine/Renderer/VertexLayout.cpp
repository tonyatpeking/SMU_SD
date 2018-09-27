#include "Engine/Renderer/VertexLayout.hpp"
#include "Engine/Renderer/GLFunctions.hpp"
#include "Engine/Core/ErrorUtils.hpp"



bool VertexLayout::HasAttribute( string name ) const
{
    for ( auto& attrib : m_attributes )
    {
        if( attrib.m_name == name )
            return true;
    }
    return false;
}

uint VertexLayout::GetAttribCountFromArray( const VertexAttribute* layout )
{
    uint MAX_ATTRIB_COUNT = 30;
    for( uint count = 0; count < MAX_ATTRIB_COUNT; ++count )
    {
        if( layout[count].m_name == VERT_LAYOUT_END )
            return count;
    }
    LOG_FATAL( "VertexLayout exceeding MAX_ATTRIB_COUNT,"
               "please make sure to pad last VertexAttribute with ::END" );
    return 0;
}

VertexAttribute VertexAttribute::END()
{
    static VertexAttribute s_end = VertexAttribute( VERT_LAYOUT_END );
    return s_end;
}
