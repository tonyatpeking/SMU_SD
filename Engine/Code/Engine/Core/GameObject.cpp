#include "Engine/Core/GameObject.hpp"
#include "Engine/Core/EngineCommon.hpp"

GameObject::~GameObject()
{
    delete m_renderable;
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
    RegenModelMatIfDirty();
    return m_renderable;
}

const Renderable* GameObject::GetRenderable() const
{
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
