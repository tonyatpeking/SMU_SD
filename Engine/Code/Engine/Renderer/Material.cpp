#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/Texture.hpp"

namespace
{
Material* s_defaultMaterial = nullptr;
}

Material* Material::CloneDefaultMaterial()
{
    if( !s_defaultMaterial )
    {
        s_defaultMaterial = new Material();
    }
    return s_defaultMaterial->Clone();
}

Material::Material()
    : m_diffuse( Texture::GetWhiteTexture() )
    , m_normal( Texture::GetFlatNormalTexture() )
    , m_shaderPass( ShaderPass::GetDefaultShader() )
{

}

Material::~Material()
{
    //#TODO Resource manager
    //delete m_shaderPass;
}

Material* Material::Clone()
{
    Material* newMat = new Material();
    *newMat = *this;
    newMat->m_isResource = false;
    return newMat;
}

void Material::SetProgram( ShaderProgram* program )
{
    m_shaderPass->m_program = program;
}

void Material::SetDiffuse( const Texture* texture )
{
    m_diffuse = texture;
}

void Material::SetNormal( const Texture* normal )
{
    m_normal = normal;
}

void Material::SetRenderState( RenderState state )
{
    m_shaderPass->m_state = state;
}

ShaderPass* Material::GetShaderPass( uint shaderPassID )
{
    UNUSED( shaderPassID );
    return m_shaderPass;//#placeholder
}

void Material::SetShaderPass( uint shaderPassID, ShaderPass* shaderPass )
{
    UNUSED( shaderPassID );
    m_shaderPass = shaderPass;//#placeholder
}

uint Material::GetProgramHandle( uint shaderPassID )
{
    UNUSED( shaderPassID );
    return m_shaderPass->m_program->GetHandle();//#placeholder
}
