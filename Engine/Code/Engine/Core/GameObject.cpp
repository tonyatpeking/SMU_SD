#include "Engine/Core/GameObject.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Core/GameObjectManager.hpp"


GameObject::GameObject( std::string type )
{
    SetType( type );
    m_manager = GameObjectManager::GetDefault();
    if( m_manager )
        m_manager->AddGameObject( this );
}

GameObject::~GameObject()
{
    SetGameObjectManager( nullptr );
    delete m_renderable;
}

void GameObject::Update()
{
    if( !m_firstUpdateCalled )
    {
        m_firstUpdateCalled = true;
        OnFirstUpdate();
    }
}

void GameObject::SetGameObjectManager( GameObjectManager* manager )
{
    if( m_manager )
        m_manager->RemoveGameObject( this );

    m_manager = manager;

    if( m_manager )
        m_manager->AddGameObject( this );
}

Transform& GameObject::GetTransform()
{
    m_modelMatDirty = true;
    return m_transform;
}

const Transform& GameObject::GetTransform() const
{
    return m_transform;
}

void GameObject::PreRender( Camera* camera )
{
    UNUSED( camera );
}

Renderable* GameObject::SetRenderable( Renderable* renderable )
{
    delete m_renderable;
    m_renderable = renderable;
    m_modelMatDirty = true;
    RegenModelMatIfDirty();
    return m_renderable;
}

Renderable* GameObject::GetRenderable()
{
    if( m_renderable )
        RegenModelMatIfDirty();
    return m_renderable;
}

const Renderable* GameObject::GetRenderable() const
{
    if( m_renderable )
        RegenModelMatIfDirty();
    return m_renderable;
}

void GameObject::RegenModelMatIfDirty() const
{
    //     if( m_modelMatDirty )
    //     {
    //         m_modelMatDirty = false;
    //     }
    m_renderable->m_modelMatrix = m_transform.GetLocalToWorld();

}

void GameObject::SetShouldDie( bool shouldDie )
{
    m_shouldDie = shouldDie;
    CallDeathCallbacks();
}

void GameObject::CallDeathCallbacks()
{
    for( GameObjectCB cb : m_deathCallbacks )
    {
        cb( this );
    }
}

void GameObject::AddDeathCallback( GameObjectCB cb )
{
    m_deathCallbacks.push_back( cb );
}

void GameObject::ClearDeathCallbacks()
{
    m_deathCallbacks.clear();
}

void GameObject::SetType( std::string type )
{
    m_type = type;
}

