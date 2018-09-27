#pragma once
#include "Engine/Core/Types.hpp"
#include "Engine/Renderer/DrawCall.hpp"

class Renderer;
class RenderSceneGraph;
class Camera;
class Light;

class ForwardRenderingPath
{
public:
    ForwardRenderingPath( Renderer* renderer )
        : m_renderer( renderer ) {};
    ~ForwardRenderingPath() {};
    void Render( RenderSceneGraph* scene );

private:

    void RenderShadowMapForLight( Light* light );
    void RenderSceneForCamera( Camera* camera );
    // lights is an array of light indices, max size is UBO::MAX_LIGHTS
    void ComputeMostContributingLights( const Vec3& pos,
                                        int out_lightIndices[] );
    void SortDrawCalls( vector<DrawCall>& out_drawCalls );
    void EnableLightsForDrawCall( const DrawCall& drawCall );

public:
    void UpdateShadowCamera( Light* light, Camera* mainCamera);

private:
    Renderer * m_renderer;
    RenderSceneGraph* m_scene;

};
