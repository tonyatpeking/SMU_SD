#include "Engine/Math/OBB3.hpp"


// face vertex order
//                7-------6
//               /|      /|
//              3-|-----2 |
//              | 4-----|-5
//              |/   o  |/
//              0-------1
// out should be 8 large
void OBB3::GetCorners( Vec3* out_corners ) const
{
    out_corners[0] = m_localToWorld.TransformPosition( m_aabb3.GetBackwardBottomLeft() );
    out_corners[1] = m_localToWorld.TransformPosition( m_aabb3.GetBackwardBottomRight() );
    out_corners[2] = m_localToWorld.TransformPosition( m_aabb3.GetBackwardTopRight() );
    out_corners[3] = m_localToWorld.TransformPosition( m_aabb3.GetBackwardTopLeft() );
    out_corners[4] = m_localToWorld.TransformPosition( m_aabb3.GetForwardBottomLeft() );
    out_corners[5] = m_localToWorld.TransformPosition( m_aabb3.GetForwardBottomRight() );
    out_corners[6] = m_localToWorld.TransformPosition( m_aabb3.GetForwardTopRight() );
    out_corners[7] = m_localToWorld.TransformPosition( m_aabb3.GetForwardTopLeft() );
}

bool OBB3::ContainsPoint( Vec3& pos ) const
{
    Vec3 localPos = GetWorldToLocal().TransformPosition( pos );
    return m_aabb3.ContainsPoint( localPos );
}

Mat4& OBB3::GetLocalToWorld()
{
    m_worldToLocalDirty = true;
    return m_localToWorld;
}

const Mat4& OBB3::GetLocalToWorld() const
{
    return m_localToWorld;
}

void OBB3::SetLocalToWorld( const Mat4& localToWorld )
{
    m_worldToLocalDirty = true;
    m_localToWorld = localToWorld;
}

const Mat4& OBB3::GetWorldToLocal() const
{
    if( m_worldToLocalDirty )
    {
        m_worldToLocal = m_localToWorld.Inverse();
        m_worldToLocalDirty = false;
    }
    return m_worldToLocal;
}
