#include "Engine/Renderer/RenderState.hpp"

const RenderState& RenderState::GetDefault()
{
    static RenderState defaultRenderState{};
    return defaultRenderState;
}




