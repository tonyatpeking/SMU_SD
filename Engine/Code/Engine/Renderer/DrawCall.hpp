#pragma once
#include "Engine/Renderer/UBO.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Core/Types.hpp"

class Mesh;
class Renderable;
class Material;

class DrawCall
{
public:
    DrawCall( Renderable* renderable, uint subMeshID, uint shaderPassID );
	~DrawCall(){};
    // Camera *m_camera;

    void SetLights( int lightIndices[] );

    Renderable* m_renderable;
    uint m_subMeshID;
    uint m_shaderPassID;

    // -1 index for unused light
    int m_lightIndices[MAX_LIGHTS];
    // Light* m_lights[MAX_LIGHTS];
    uint m_sortOrder;
    uint m_queue;

};
