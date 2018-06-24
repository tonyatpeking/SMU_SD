#pragma once
#include <functional>

#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"

class Camera;
class RenderSceneGraph;
class GameObject;

typedef std::function<void( GameObject* )> GameObjectCB;

class GameObject
{
public:
    // GameObject will default to RenderSceneGraph::GetCurrentScene()
    GameObject();
    virtual ~GameObject();

    virtual void Update() {};

    virtual void SetScene( RenderSceneGraph* scene );


    Transform& GetTransform();
    const Transform& GetTransform() const;

    virtual void PreRender( Camera* camera );
    Renderable* SetRenderable( Renderable* renderable );
    Renderable* GetRenderable();
    const Renderable* GetRenderable() const;
    void RegenModelMatIfDirty() const;

    template <typename T>
    void SetParent( T* parent )
    {
        GetTransform().SetParent( &parent->GetTransform() );
    }

    void SetShouldDie( bool shouldDie );
    bool ShouldDie() { return m_shouldDie; };
    void CallDeathCallbacks();
    void AddDeathCallback( GameObjectCB cb );
    void ClearDeathCallbacks();

protected:

    Transform m_transform;
    mutable bool m_modelMatDirty = false;
    mutable Renderable* m_renderable;

    bool m_shouldDie = false;
    RenderSceneGraph* m_scene = nullptr;

    std::vector < GameObjectCB > m_deathCallbacks;
};
