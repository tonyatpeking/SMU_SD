#include "Engine/Renderer/FrameBuffer.hpp"
#include "Engine/Renderer/GLFunctions.hpp"
#include "Engine/Renderer/Texture.hpp"

FrameBuffer::FrameBuffer()
{
    glGenFramebuffers( 1, &m_handle );
}

FrameBuffer::~FrameBuffer()
{
    if( m_handle != NULL )
        glDeleteFramebuffers( 1, &m_handle );
}

void FrameBuffer::SetColorTarget( Texture *colorTarget )
{
     m_colorTarget = colorTarget;
     m_dimensions = colorTarget->GetDimensions();
}

bool FrameBuffer::Finalize()
{
    glBindFramebuffer( GL_FRAMEBUFFER, m_handle );


    if( m_colorTarget )
    {
        // keep track of which outputs go to which attachments;
        GLenum targets[1];

        // Finalize a color target to an attachment point
        // and keep track of which locations to to which attachments.
        glFramebufferTexture( GL_FRAMEBUFFER,
                              GL_COLOR_ATTACHMENT0 + 0,
                              m_colorTarget->GetHandle(),
                              0 );
        // 0 to to attachment 0
        targets[0] = GL_COLOR_ATTACHMENT0 + 0;

        // Update target bindings
        glDrawBuffers( 1, targets );
    }
    else
    {
        glDrawBuffer( GL_NONE );
    }


    // Finalize depth if available;
    if( m_depthStencilTarget == nullptr )
    {
        glFramebufferTexture( GL_FRAMEBUFFER,
                              GL_DEPTH_STENCIL_ATTACHMENT,
                              NULL,
                              0 );
    }
    else
    {
        glFramebufferTexture( GL_FRAMEBUFFER,
                              GL_DEPTH_STENCIL_ATTACHMENT,
                              m_depthStencilTarget->GetHandle(),
                              0 );
    }

    // Error Check - recommend only doing in debug
#if defined(_DEBUG)
    GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
    if( status != GL_FRAMEBUFFER_COMPLETE )
    {
        String msg = Stringf( "Failed to create framebuffer:  %u", status );
        LOG_WARNING( msg );
        return false;
    }
#endif

    return true;
}
