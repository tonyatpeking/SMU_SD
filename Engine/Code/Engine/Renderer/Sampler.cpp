#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/GLFunctions.hpp"

Sampler* Sampler::GetPointSampler()
{
    static Sampler* s_sampler = nullptr;
    if( !s_sampler )
    {
        s_sampler = new Sampler();
        uint handle = s_sampler->m_samplerHandle;
        glSamplerParameteri( handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glSamplerParameteri( handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    }
    return s_sampler;
}

Sampler* Sampler::GetTrilinearSampler()
{
    static Sampler* s_sampler = nullptr;
    if( !s_sampler )
    {
        s_sampler = new Sampler();
        uint handle = s_sampler->m_samplerHandle;
        glSamplerParameteri( handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glSamplerParameteri( handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
    return s_sampler;
}

Sampler::Sampler()
{
    Create();
}

Sampler::~Sampler()
{

}

bool Sampler::Create()
{
    // create the sampler handle if needed;
    if( m_samplerHandle == NULL )
    {
        glGenSamplers( 1, &m_samplerHandle );
        if( m_samplerHandle == NULL )
        {
            return false;
        }
    }

    // setup wrapping
    glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_R, GL_REPEAT );

    // filtering;
    glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    return true;
}

void Sampler::Destroy()
{
    if( m_samplerHandle != NULL )
    {
        glDeleteSamplers( 1, &m_samplerHandle );
        m_samplerHandle = NULL;
    }
}
