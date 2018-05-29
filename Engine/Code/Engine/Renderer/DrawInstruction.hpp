#pragma once
#include "Engine/Renderer/RendererEnums.hpp"

struct DrawInstruction
{
    uint m_elemCount = 0;
    DrawPrimitive m_drawPrimitive = DrawPrimitive::TRIANGLES;
    bool m_useIndices = true;
    uint m_startIdx = 0;
};
