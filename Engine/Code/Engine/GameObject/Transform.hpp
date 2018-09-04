#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat4.hpp"

class Camera;


class Transform
{
public:
    Transform() {};
    ~Transform();

    // don't copy children
    Transform( const Transform& copyFrom );

    // don't copy children
    void operator=( const Transform& copyFrom );

    void SetIdentity();

    Vec3 GetForward() const;
    Vec3 GetUp() const;
    Vec3 GetRight() const;

    Vec3 GetLocalForward() const;
    Vec3 GetLocalUp() const;
    Vec3 GetLocalRight() const;

    // Matrices
    const Mat4& GetLocalToParent() const;
    void SetLocalToParent( const Mat4& mat );

    const Mat4& GetLocalToWorld() const;

    const Mat4& GetWorldToParent() const;
    const Mat4& GetParentToWorld() const;

    // Inverse Matrices
    const Mat4& GetWorldToLocal() const;
    const Mat4& GetParentToLocal() const;

    // Position
    Vec3 GetLocalPosition() const;
    void SetLocalPosition( const Vec3& position );
    void TranslateLocal( const Vec3& translation );

    Vec3 GetWorldPosition() const;
    void SetWorldPosition( const Vec3& position );
    void TranslateWorld( const Vec3& translation );

    // Rotation
    // roll first, then pitch, then yaw
    Vec3 GetLocalEuler() const;
    void SetLocalEuler( const Vec3& euler );
    void RotateLocalEuler( const Vec3& euler );

    Vec3 GetWorldEuler() const;
    void SetWorldEuler( const Vec3& euler );
    void RotateWorldEuler( const Vec3& euler );

    // Scale
    Vec3 GetLocalScale() const;
    void SetLocalScale( const Vec3& scale );
    void SetLocalScaleUniform( float uniformScale );
    Vec3 GetWorldScale() const;

    void LookAt( const Vec3& target, const Vec3& worldUp = Vec3::UP );
    void FaceCamera( const Camera* cam );


    //-----------------------------------------------------------------------------------
    // Transform tree
    bool HasParent() const;
    bool HasChildren() const;
    Transform* GetParent() { return m_parent; };
    std::vector<Transform*>& GetChildren() { return m_children; };
    // parent can be null
    void SetParentKeepWorldTransform( Transform* parent );
    void SetParent( Transform* parent );
private:
    void SetChildrenLocalToWorldDirty( bool dirty ) const;
    void SetLocalToParentDirty( bool dirty ) const;
    void SetLocalToWorldDirty( bool dirty ) const;
    void SetWorldToLocalDirty( bool dirty ) const;
    void SetParentToLocalDirty( bool dirty ) const;
    void SetLocalToWorldSRTDirty( bool dirty ) const;

    // regenerate m_localMatrix if m_transformData has been touched
    void RegenLocalToWorldIfDirty() const;
    void RegenLocalToParentIfDirty() const;
    void RegenWorldSRTIfDirty() const;

    void AddChild( Transform* child );
    void RemoveChild( Transform* child );
    // holds local to parent data
    //TransformData m_transformData;
    // mutable so we can cache these in const functions

    mutable bool m_localToParentDirty = false;
    mutable Mat4 m_localToParent;
    mutable bool m_parentToLocalDirty = false;
    mutable Mat4 m_parentToLocal;

    mutable bool m_localToWorldDirty = false;
    mutable Mat4 m_localToWorld; // aka model
    mutable bool m_worldToLocalDirty = false;
    mutable Mat4 m_worldToLocal;

//     mutable Mat4 m_localMatrix;

    Transform* m_parent = nullptr;
    std::vector<Transform*> m_children;

    // These are always up to date after a function call so do not need dirty flags
    Vec3 m_localPosition = Vec3::ZEROS;
    Vec3 m_localEuler = Vec3::ZEROS;
    Vec3 m_localScale = Vec3::ONES;

    mutable bool m_worldSRTDirty = false;
    mutable Vec3 m_worldPosition = Vec3::ZEROS;
    mutable Vec3 m_worldEuler = Vec3::ZEROS;
    mutable Vec3 m_worldScale = Vec3::ONES;
};
