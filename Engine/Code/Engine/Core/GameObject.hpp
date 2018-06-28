#pragma once
#include <functional>
#include <string>

#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"

class Camera;
class RenderSceneGraph;
class GameObject;
class GameObjectManager;
class AABB3;
class OBB3;

typedef std::function<void( GameObject* )> GameObjectCB;

class GameObject
{
public:
    // GameObject will default to RenderSceneGraph::GetDefault()
    GameObject( std::string type = "Unknown" );
    virtual ~GameObject();

    virtual void Update();
    virtual void OnFirstUpdate() {};

    void SetGameObjectManager( GameObjectManager* manager );

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

    void SetType( std::string type );
    std::string GetType() { return m_type; };

    OBB3 GetWorldBounds() const;
    AABB3 GetLocalBounds() const;

protected:
    std::string m_type = "Unknown";

    Transform m_transform;
    mutable bool m_modelMatDirty = false;
    mutable Renderable* m_renderable;

    bool m_shouldDie = false;
    RenderSceneGraph* m_scene = nullptr;
    GameObjectManager* m_manager = nullptr;

    std::vector < GameObjectCB > m_deathCallbacks;

    bool m_firstUpdateCalled = false;
};
