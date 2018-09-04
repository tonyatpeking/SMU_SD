#pragma once
#include "Engine/Core/Types.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/DataUtils/Blob.hpp"

class UniformBuffer
{
public:
    UniformBuffer() {};
    ~UniformBuffer() {};

public:
    // copy data to the CPU and dirty the buffer
    void SetCpuData( size_t byteCount, void const *data );

    // update the gpu buffer from the local cpu buffer if dirty
    // and clears the dirty flag
    void UpdateGpu();

    // sets the cpu and gpu buffers - clears the dirty flag
    void SetGpuData( size_t byteCount, void const *data );

    // gets a pointer to the cpu buffer (const - so does not dirty)
    void const* GetCpuBuffer() const;

    // get a mutable pointer to the cpu buffer.  Sets the dirty flag
    // as it expects the user to change it.
    void* GetCpuBuffer();

    // get this buffer's size in bytes
    size_t GetByteCount() const;

    uint GetHandle();

    void BindToSlot( uint uboSlot );

    // template helpers
public:
    //------------------------------------------------------------------------
    // let's me just set a structure, and it'll figure out the size
    template <typename T>
    void Set( T const &v )
    {
        SetGpuData( sizeof( T ), &v );
    }

    //------------------------------------------------------------------------
    // gets a constant reference to the CPU buffer as known struct
    // would get the same as saying (T*) uniform_buffer->get_cpu_buffer();
    template <typename T>
    T const* As() const
    {
        return (T const*) GetCpuBuffer();
    }

    //------------------------------------------------------------------------
    template <typename T>
    T* As()
    {
        return (T*) GetCpuBuffer();
    }

public:
    // render buffer backing this constant buffer (gpu memory
    RenderBuffer m_renderBuffer = RenderBuffer( BufferTarget::UNIFORM_BUFFER );
    // cpu copy of the data stored in m_render-buffer
    Blob m_cpuBuffer;

    // uses to check if we should update gpu from cpu
    bool m_dirtyBit = false;

public:
    //------------------------------------------------------------------------
    //
    template <typename T>
    static UniformBuffer* For( T const &v )
    {
        // Exercise - convenience constructor,
        // will create a constant buffer for a specific struct,
        // initializing to the structs contents;
        UNIMPLEMENTED();

        /* usage example
        time_buffer_t time_buffer;
        // fill time_buffer with data
        m_time_ubo = UniformBuffer::For( time_buffer );
        */

    }
};