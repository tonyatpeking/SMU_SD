#include "Engine/Renderer/DrawCall.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Renderer/Material.hpp"

DrawCall::DrawCall( Renderable* renderable, uint subMeshID, uint shaderPassID )
    : m_renderable( renderable )
    , m_subMeshID( subMeshID )
    , m_shaderPassID( shaderPassID )
{
    ShaderPass* shaderPass = renderable->GetMaterial( subMeshID )
        ->GetShaderPass( shaderPassID );
    m_sortOrder = shaderPass->m_sortOrder;
    m_queue = (uint) shaderPass->m_queue;
}

void DrawCall::SetLights( int lightIndices[] )
{
    for( int idx = 0; idx < MAX_LIGHTS; ++idx )
    {
        m_lightIndices[idx] = lightIndices[idx];
    }
}
