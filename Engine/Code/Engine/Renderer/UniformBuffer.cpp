#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/GLFunctions.hpp"


void UniformBuffer::SetCpuData( size_t byteCount, void const *data )
{
    m_cpuBuffer.CopyData( byteCount, data );
    m_dirtyBit = true;
}

void UniformBuffer::UpdateGpu()
{
    m_renderBuffer.CopyToGPU( m_cpuBuffer.m_byteCount, m_cpuBuffer.m_data );
    m_dirtyBit = false;
}

void UniformBuffer::SetGpuData( size_t byteCount, void const *data )
{
    SetCpuData( byteCount, data );
    UpdateGpu();
}

void const* UniformBuffer::GetCpuBuffer() const
{
    return m_cpuBuffer.m_data;
}

void* UniformBuffer::GetCpuBuffer()
{
    m_dirtyBit = true;
    return m_cpuBuffer.m_data;
}

size_t UniformBuffer::GetByteCount() const
{
    return m_cpuBuffer.m_byteCount;
}

uint UniformBuffer::GetHandle()
{
    return m_renderBuffer.GetHandle();
}

void UniformBuffer::BindToSlot( uint uboSlot )
{
    m_renderBuffer.BindBuffer();
    glBindBufferBase( GL_UNIFORM_BUFFER, uboSlot, GetHandle() );
}

