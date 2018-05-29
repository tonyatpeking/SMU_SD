#pragma once
#include "Engine/Renderer/RendererEnums.hpp"

struct RenderState
{
    const static RenderState& GetDefault();
public:
    // Raster state
    CullMode m_cullMode = CullMode::BACK;
    FillMode m_fillMode = FillMode::SOLID;
    WindOrder m_windOrder = WindOrder::COUNTER_CLOCKWISE;

    // Depth state
    DepthCompareMode m_depthCompare = DepthCompareMode::LESS;
    bool m_depthWrite = true;

    // blend
    bool m_enableBlend = true;
    BlendOP m_blendOP = BlendOP::ADD;
    BlendFactor m_srcFactor = BlendFactor::ONE;
    BlendFactor m_dstFactor = BlendFactor::ZERO;
};

