#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IVec2.hpp"

class Texture;

class FrameBuffer
{
public:
    FrameBuffer();
    ~FrameBuffer();

    // should just update members
    // finalize does the actual binding
    void SetColorTarget( Texture *colorTarget );
    void SetDepthStencilTarget( Texture *depthTarget ) { m_depthStencilTarget = depthTarget; };

    // setups the the GL framebuffer - called before us.
    // TODO: Make sure this only does work if the targets
    // have changed.
    bool Finalize();
    uint GetHandle() { return m_handle; };

    IVec2 GetDimensions() const { return m_dimensions; };
    float GetAspect() const { return  (float) m_dimensions.x / (float) m_dimensions.y; };
private:
    IVec2 m_dimensions;
    uint m_handle;
    Texture *m_colorTarget = nullptr;
    Texture *m_depthStencilTarget = nullptr;
};