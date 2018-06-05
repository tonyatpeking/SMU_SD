#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Renderer/Renderable.hpp"

RenderSceneGraph* RenderSceneGraph::s_currentScene = nullptr;

void RenderSceneGraph::AddGameObject( GameObject* gameObject )
{
    m_gameObjects.push_back( gameObject );
}

void RenderSceneGraph::AddLight( Light* light )
{
    m_lights.push_back( light );
}

void RenderSceneGraph::AddCamera( Camera* camera )
{
    m_cameras.push_back( camera );
}

void RenderSceneGraph::RemoveGameObject( GameObject* gameObject )
{
    ContainerUtils::EraseOneValue( m_gameObjects, gameObject );
}

void RenderSceneGraph::RemoveLight( Light* light )
{
    ContainerUtils::EraseOneValue( m_lights, light );
}

void RenderSceneGraph::RemoveCamera( Camera* camera )
{
    ContainerUtils::EraseOneValue( m_cameras, camera );
}

std::vector<Renderable*>& RenderSceneGraph::GetRenderables()
{
    m_renderables.clear();
    m_renderables.reserve( m_gameObjects.size() );
    for ( auto& gameObject : m_gameObjects )
    {
        Renderable* renderable = gameObject->GetRenderable();
        if( renderable )
            m_renderables.push_back( renderable );
    }
    return m_renderables;
}


