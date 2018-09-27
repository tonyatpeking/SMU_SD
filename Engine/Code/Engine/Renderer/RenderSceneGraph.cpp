#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/GameObject/GameObjectManager.hpp"

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

vector<GameObject*>& RenderSceneGraph::GetGameObjects()
{
    return m_manager->GetObejctsFlat();
}

vector<Renderable*>& RenderSceneGraph::GetRenderables()
{
    m_renderables.clear();
    m_renderables.reserve( GetGameObjects().size() );
    for ( auto& gameObject : GetGameObjects() )
    {
        if( !gameObject->IsVisible() )
            continue;
        Renderable* renderable = gameObject->GetRenderable();
        if( renderable )
            m_renderables.push_back( renderable );
    }
    return m_renderables;
}

vector<GameObject*>& RenderSceneGraph::GetLights()
{
    return m_manager->GetObjectsOfType( "Light" );
}

void RenderSceneGraph::RemoveCamera( Camera* camera )
{
    ContainerUtils::EraseOneValue( m_cameras, camera );
}

vector<Camera*>& RenderSceneGraph::GetCameras()
{
    return m_cameras;
}

