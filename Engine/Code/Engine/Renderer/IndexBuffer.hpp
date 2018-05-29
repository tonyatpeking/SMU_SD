#pragma once

#include "Engine/Renderer/RenderBuffer.hpp"

class IndexBuffer : public RenderBuffer
{
public:
    IndexBuffer()
        : RenderBuffer( BufferTarget::INDEX_BUFFER ) {};
    ~IndexBuffer() {};

    void SetIndices( uint count, const uint* indices );

    uint m_indexCount;

private:

};
