﻿#pragma once

#include "Engine/Core/WindowsCommon.hpp"

#include "Engine/Core/SmartEnum.hpp"

#include "Engine/Renderer/GLFunctions.hpp"

class Mesh;
class Material;
class ShaderProgram;
class Sampler;
class Window;

class RenderingContext
{
public:
    RenderingContext( Window* window );
    ~RenderingContext();

private:
    static HGLRC CreateOldRenderContext( HDC hdc );
    static HGLRC CreateRealRenderContext( HDC hdc, int major, int minor );

    bool Init();
    void Cleanup();

    HGLRC m_GLContext = NULL;    // our rendering context;
    Window* m_window = nullptr;
};
