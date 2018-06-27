#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Core/GameObjectManager.hpp"

RenderSceneGraph* RenderSceneGraph::s_default = nullptr;


RenderSceneGraph* RenderSceneGraph::GetDefault()
{
    if( !s_default )
        s_default = new RenderSceneGraph();

    return s_default;
}

RenderSceneGraph::RenderSceneGraph()
{
    m_manager = GameObjectManager::GetDefault();
}

std::vector<GameObject*>& RenderSceneGraph::GetGameObjects()
{
    return m_manager->GetObejctsFlat();
}

std::vector<Renderable*>& RenderSceneGraph::GetRenderables()
{
    m_renderables.clear();
    m_renderables.reserve( GetGameObjects().size() );
    for ( auto& gameObject : GetGameObjects() )
    {
        Renderable* renderable = gameObject->GetRenderable();
        if( renderable )
            m_renderables.push_back( renderable );
    }
    return m_renderables;
}

std::vector<GameObject*>& RenderSceneGraph::GetLights()
{
    return m_manager->GetObjectsOfType( "Light" );
}

void RenderSceneGraph::RemoveCamera( Camera* camera )
{
    ContainerUtils::EraseOneValue( m_cameras, camera );
}

std::vector<Camera*>& RenderSceneGraph::GetCameras()
{
    return m_cameras;
}

