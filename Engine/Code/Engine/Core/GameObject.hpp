#pragma once
#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"

class Camera;

class GameObject
{
public:
    GameObject() {};
    virtual ~GameObject();

    virtual void Update() {};

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

    void SetShouldDie( bool shouldDie ) { m_shouldDie = shouldDie; };
    bool ShouldDie() { return m_shouldDie; };

protected:

    Transform m_transform;
    mutable bool m_modelMatDirty = false;
    mutable Renderable* m_renderable;

    bool m_shouldDie = false;

};
