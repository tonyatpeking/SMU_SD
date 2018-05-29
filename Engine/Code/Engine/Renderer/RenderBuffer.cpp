#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/GLFunctions.hpp"


namespace
{

GLenum g_glBufferTarget[BufferTarget::COUNT] =
{
    GL_ARRAY_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER,
    GL_UNIFORM_BUFFER
};

GLenum GLBufferTarget( BufferTarget bufferTarget )
{
    return g_glBufferTarget[bufferTarget];
}

}

RenderBuffer::RenderBuffer( BufferTarget bufferTarget )
    : m_bufferTarget( bufferTarget )
{

}

RenderBuffer::~RenderBuffer()
{
    glDeleteBuffers( 1, &m_handle );
}

bool RenderBuffer::CopyToGPU( const size_t byteCount, const void* data )
{
    // m_handle is a GLuint member - used by OpenGL to identify this buffer
    // if we don't have one, make one when we first need it [lazy instantiation]
    if( m_handle == NULL )
    {
        glGenBuffers( 1, &m_handle );
    }

    // Finalize the buffer to a slot, and copy memory
    // GL_DYNAMIC_DRAW means the memory is likely going to change a lot (we'll get
    // during the second project)
    glBindBuffer( GLBufferTarget( m_bufferTarget ), m_handle );
    glBufferData( GLBufferTarget( m_bufferTarget ), byteCount, data, GL_DYNAMIC_DRAW );

    // buffer_size is a size_t member variable I keep around for
    // convenience
    m_bufferSize = byteCount;
    return true;
}

void RenderBuffer::BindBuffer()
{
    glBindBuffer( GLBufferTarget( m_bufferTarget ), m_handle );
}

