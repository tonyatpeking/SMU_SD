#pragma once
#include <functional>
#include <string>

#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec3.hpp"

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
    // pivot is in local space (-1,-1,-1) to (1,1,1) for cube min and max corners
    static GameObject* MakeCube( const Vec3& sideLengths = Vec3::ONES,
                                 const Vec3& pivot = Vec3::ZEROS );

    // GameObject RenderSceneGraph will default to RenderSceneGraph::GetDefault()
    GameObject( const String& type = "Unknown" );
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

    template <typename T>
    void SetParentKeepWorldTransform( T* parent )
    {
        GetTransform().SetParentKeepWorldTransform( &parent->GetTransform() );
    }

    void SetShouldDie( bool shouldDie );
    bool ShouldDie() { return m_shouldDie; };
    void CallDeathCallbacks();
    virtual void OnDeath() {};
    void AddDeathCallback( GameObjectCB cb );
    void ClearDeathCallbacks();

    void SetType( std::string type );
    String GetType() { return m_type; };

    OBB3 GetOBB3() const;
    AABB3 GetLocalBounds() const;

    void SetVisible( bool visible ) { m_visible = visible; };
    bool IsVisible() { return m_visible; };



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
    bool m_visible = true;
};
