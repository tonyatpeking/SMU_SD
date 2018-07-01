#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
namespace
{

const String LIGHTING_DEBUG_SHADER = "Data/Shaders/Debug/LightingDebug";


}


ShaderPass* ShaderPass::GetDefaultShader()
{
    static ShaderPass* s_defaultShader = nullptr;
    if( !s_defaultShader )
    {
        s_defaultShader = new ShaderPass();
        s_defaultShader->EnableBlending( BlendMode::ALPHA_BLEND );
        s_defaultShader->SetProgram( ShaderProgram::GetDefaultProgram() );
    }
    return s_defaultShader;
}

ShaderPass* ShaderPass::GetDefaultUIShader()
{
    static ShaderPass* s_defaultUIShader = nullptr;
    if( !s_defaultUIShader )
    {
        s_defaultUIShader = new ShaderPass();
        s_defaultUIShader->EnableBlending( BlendMode::ALPHA_BLEND );
        s_defaultUIShader->DisableDepth();
        s_defaultUIShader->SetProgram( ShaderProgram::GetDefaultProgram() );
    }
    return s_defaultUIShader;
}

ShaderPass* ShaderPass::GetInvalidShader()
{
    static ShaderPass* s_invalidShader = nullptr;
    if( !s_invalidShader )
    {
        s_invalidShader = new ShaderPass();
        s_invalidShader->SetProgram( ShaderProgram::GetInvalidProgram() );
    }
    return s_invalidShader;
}

ShaderPass* ShaderPass::GetLightingDebugShader()
{
    static ShaderPass* s_lightingDebugShader = nullptr;
    if( !s_lightingDebugShader )
    {
        s_lightingDebugShader = new ShaderPass();
        s_lightingDebugShader->SetProgram( ShaderProgram::CreateOrGetFromFiles(
            LIGHTING_DEBUG_SHADER ) );
        s_lightingDebugShader->GetProgram()->m_isDebugProgram = true;
    }
    return s_lightingDebugShader;
}

ShaderPass* ShaderPass::GetWireframeDebugShader()
{
    static ShaderPass* s_wireframeDebugShader = nullptr;
    if( !s_wireframeDebugShader )
    {
        s_wireframeDebugShader = new ShaderPass();
        s_wireframeDebugShader->SetProgram( ShaderProgram::GetDebugProgram() );
        s_wireframeDebugShader->SetFillMode( FillMode::WIRE );
    }
    return s_wireframeDebugShader;
}

ShaderPass* ShaderPass::GetDepthOnlyShader()
{
    static ShaderPass* s_shader = nullptr;
    if( !s_shader )
    {
        s_shader = new ShaderPass();
        s_shader->SetProgram( ShaderProgram::CreateOrGetFromFiles(
            "Data/Shaders/Skybox" ) );
        s_shader->SetQueue( RenderQueue::SKY_BOX );
        RenderState& state = s_shader->GetRenderState();
        state.m_depthCompare = DepthCompareMode::LEQUAL;
        state.m_depthWrite = false;
        state.m_cullMode = CullMode::NONE;
    }
    return s_shader;
}

ShaderPass* ShaderPass::GetAdditiveShader()
{
    static ShaderPass* s_additiveShader = nullptr;
    if( !s_additiveShader )
    {
        s_additiveShader = new ShaderPass();
        s_additiveShader->SetProgram( ShaderProgram::GetDefaultProgram() );
        s_additiveShader->SetQueue( RenderQueue::ADDITIVE );
        RenderState& state = s_additiveShader->GetRenderState();
        state.m_dstFactor = BlendFactor::ONE;
        state.m_srcFactor = BlendFactor::SRC_ALPHA;
        state.m_depthWrite = false;
        state.m_cullMode = CullMode::NONE;
    }
    return s_additiveShader;
}

ShaderPass* ShaderPass::GetSkyboxShader()
{
    static ShaderPass* s_shader = nullptr;
    if( !s_shader )
    {
        s_shader = new ShaderPass();
        s_shader->SetProgram( ShaderProgram::CreateOrGetFromFiles(
            "Data/Shaders/Skybox" ) );
        s_shader->SetQueue( RenderQueue::SKY_BOX );
        RenderState& state = s_shader->GetRenderState();
        state.m_depthCompare = DepthCompareMode::LEQUAL;
        state.m_depthWrite = false;
        state.m_cullMode = CullMode::NONE;
    }
    return s_shader;
}

ShaderPass* ShaderPass::GetLitShader()
{
    static ShaderPass* s_shader = nullptr;
    if( !s_shader )
    {
        s_shader = new ShaderPass();
        s_shader->SetProgram( ShaderProgram::GetLitProgram() );
        s_shader->EnableBlending( BlendMode::ALPHA_BLEND );
    }
    return s_shader;
}

ShaderPass::ShaderPass()
{
    SetProgram( ShaderProgram::GetDefaultProgram() );
}

void ShaderPass::BindProgram()
{
    m_program->Bind();
}

uint ShaderPass::GetProgramHandle()
{
    return m_program->GetHandle();
}

void ShaderPass::EnableBlending( BlendOP op, BlendFactor src, BlendFactor dst )
{
    m_state.m_enableBlend = true;
    m_state.m_blendOP = op;
    m_state.m_srcFactor = src;
    m_state.m_dstFactor = dst;
}

void ShaderPass::EnableBlending( BlendMode blendMode )
{
    switch( blendMode )
    {
    case BlendMode::ADDITIVE:
        EnableBlending( BlendOP::ADD, BlendFactor::SRC_ALPHA, BlendFactor::ONE );
        break;
    case BlendMode::ALPHA_BLEND:
        EnableBlending( BlendOP::ADD, BlendFactor::SRC_ALPHA,
                        BlendFactor::ONE_MINUS_SRC_ALPHA );
        break;
    default:
        break;
    }
}

void ShaderPass::DisableBlending()
{
    m_state.m_enableBlend = false;
}

void ShaderPass::EnableDepth( DepthCompareMode compare /*= DepthCompareMode::LESS*/, bool write /*= true */ )
{
    m_state.m_depthCompare = compare;
    m_state.m_depthWrite = write;
}

void ShaderPass::DisableDepth()
{
    EnableDepth( DepthCompareMode::ALWAYS, false );
}


