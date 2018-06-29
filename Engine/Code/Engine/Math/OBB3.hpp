#pragma once

#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Mat4.hpp"

class Vec3;

class OBB3
{
public:
    OBB3() {};
    OBB3( const AABB3& aabb3, const Mat4& localToWorld )
        : m_aabb3( aabb3 )
        , m_localToWorld( localToWorld )
    {};
    ~OBB3() {};

    void GetCorners( Vec3* out_corners ) const;
    bool ContainsPoint( Vec3& pos ) const;

    Mat4& GetLocalToWorld();
    const Mat4& GetLocalToWorld() const;
    void SetLocalToWorld( const Mat4& localToWorld );

    const Mat4& GetWorldToLocal() const;

    AABB3 GetAABB3() const { return m_aabb3; };

    // all face planes, normals point outward
    Plane Right() const;
    Plane Left() const;
    Plane Top() const;
    Plane Bottom() const;
    Plane Forward() const;
    Plane Backward() const;

private:
    AABB3 m_aabb3;
    Mat4 m_localToWorld;
    mutable bool m_worldToLocalDirty = true;
    mutable Mat4 m_worldToLocal;

};
