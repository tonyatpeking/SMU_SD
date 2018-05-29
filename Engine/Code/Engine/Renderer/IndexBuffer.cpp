#include "Engine/Renderer/IndexBuffer.hpp"


void IndexBuffer::SetIndices( uint count, const uint* indices )
{
    m_indexCount = count;
    CopyToGPU( count * sizeof( uint ), indices );
}
