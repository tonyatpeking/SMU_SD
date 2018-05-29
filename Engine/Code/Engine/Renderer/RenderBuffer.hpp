#pragma once

#include "Engine/Core/SmartEnum.hpp"
#include "Engine/Core/EngineCommon.hpp"

SMART_ENUM
(
    BufferTarget,

    INVALID = -1,
    ARRAY_BUFFER,
    INDEX_BUFFER,
    UNIFORM_BUFFER
)

class RenderBuffer
{
public:
    RenderBuffer( BufferTarget bufferTarget = BufferTarget::ARRAY_BUFFER );
    virtual ~RenderBuffer();
    bool CopyToGPU( const size_t byteCount, const void* data );
    uint GetHandle() const { return m_handle; };
    void BindBuffer();
    void SetBufferTarget( BufferTarget bufferTarget ) { m_bufferTarget = bufferTarget; };
    size_t GetBufferSize() const { return m_bufferSize; };
private:
    uint m_handle = NULL;
    size_t m_bufferSize = 0;
    BufferTarget m_bufferTarget = BufferTarget::INVALID;
};
