#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"

class VertexBuffer : public RenderBuffer
{
public:
    VertexBuffer()
        : RenderBuffer( BufferTarget::ARRAY_BUFFER ) {};
    ~VertexBuffer() {};

    template<typename T>
    void SetVertices( uint count, const T* verts )
    {
        m_vertCount = count;
        m_vertStride = count * sizeof( T );
        CopyToGPU( count * m_vertStride, verts );
    }

    uint m_vertCount;
    uint m_vertStride;

private:

};
