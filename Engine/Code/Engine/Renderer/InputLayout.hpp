#pragma once
#include "Engine/Core/Types.hpp"

class Mesh;
class ShaderProgram;

// AKA VAO
struct InputLayout
{
    static InputLayout& GetGlobalInputLayout();
    uint programHandle = 0;
    uint m_vaoID = 0;
};

