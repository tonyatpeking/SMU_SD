#include "Engine/Renderer/InputLayout.hpp"
#include "Engine/Renderer/GLFunctions.hpp"


InputLayout& InputLayout::GetGlobalInputLayout()
{
    static InputLayout debugInputLayout;
    if( debugInputLayout.m_vaoID == 0 )
    {
        glGenVertexArrays( 1, &debugInputLayout.m_vaoID );
    }
    return debugInputLayout;
}
