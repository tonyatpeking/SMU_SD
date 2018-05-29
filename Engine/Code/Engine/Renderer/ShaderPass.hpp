#pragma once
#include "Engine/Renderer/RendererEnums.hpp"
#include "Engine/Renderer/RenderState.hpp"
#include "Engine/Core/SmartEnum.hpp"
// defined in wingdi.h
#undef OPAQUE

SMART_ENUM(
    RenderQueue,

    OPAQUE,
    SKY_BOX,
    ADDITIVE,
    ALPHA,
    POST_PROCESSING
)

class ShaderProgram;

class ShaderPass
{
public:

    static ShaderPass* GetDefaultShader();
    static ShaderPass* GetDefaultUIShader();
    static ShaderPass* GetInvalidShader();
    static ShaderPass* GetLightingDebugShader();
    static ShaderPass* GetWireframeDebugShader();
    static ShaderPass* GetAdditiveShader();
    static ShaderPass* GetSkyboxShader();

    ShaderPass();

    void SetProgram( ShaderProgram* program ) { m_program = program; };
    ShaderProgram* GetProgram() { return m_program; };
    void BindProgram();
    uint GetProgramHandle();

    void EnableBlending( BlendMode blendMode );
    void EnableBlending( BlendOP op, BlendFactor src, BlendFactor dst );
    void DisableBlending();

    void EnableDepth( DepthCompareMode compare = DepthCompareMode::LESS,
                      bool write = true );
    void DisableDepth();

    void SetCullMode( CullMode cull ) { m_state.m_cullMode = cull; };
    void SetFillMode( FillMode fill ) { m_state.m_fillMode = fill; };
    void SetWindOrder( WindOrder windOrder ) { m_state.m_windOrder = windOrder; };

    void SetUsesLights( bool usesLights ) { m_usesLights = usesLights; };
    bool UsesLights() { return m_usesLights; };

    void SetQueue( RenderQueue queue ) { m_queue = queue; };
    RenderQueue GetQueue() { return m_queue; };
    void SetSortOrder( int order ) { m_sortOrder = order; };
    int GetSortOrder() { return m_sortOrder; };

    RenderState& GetRenderState() { return m_state; };
    void SetRenderState( RenderState state ) { m_state = state; };

public:
    bool m_usesLights = true;
    ShaderProgram * m_program = nullptr;
    RenderState m_state;
    int m_sortOrder = 0;
    RenderQueue m_queue = RenderQueue::OPAQUE;
};
