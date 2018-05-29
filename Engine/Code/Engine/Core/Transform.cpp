#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ContainerUtils.hpp"

void Transform::SetIdentity()
{
    m_localToParent = Mat4::IDENTITY;
    m_localPosition = Vec3::ZEROS;
    m_localEuler = Vec3::ZEROS;
    m_localScale = Vec3::ONES;
    SetLocalToParentDirty( false );
    SetLocalToWorldDirty( true );
}

Vec3 Transform::GetForward() const
{
    return Vec3( GetLocalToWorld().K );
}

Vec3 Transform::GetUp() const
{
    return Vec3( GetLocalToWorld().J );
}

Vec3 Transform::GetRight() const
{
    return Vec3( GetLocalToWorld().I );
}

Vec3 Transform::GetLocalForward() const
{
    return Vec3( GetLocalToParent().K );
}

Vec3 Transform::GetLocalUp() const
{
    return Vec3( GetLocalToParent().J );
}

Vec3 Transform::GetLocalRight() const
{
    return Vec3( GetLocalToParent().I );
}

const Mat4& Transform::GetLocalToParent() const
{
    RegenLocalToParentIfDirty();
    return m_localToParent;
}

void Transform::SetLocalToParent( const Mat4& mat )
{
    m_localToParent = mat;
    mat.DecomposeToSRT( m_localScale, m_localEuler, m_localPosition );
    SetLocalToParentDirty( false );
    SetLocalToWorldDirty( true );
}

const Mat4& Transform::GetLocalToWorld() const
{
    RegenLocalToWorldIfDirty();
    return m_localToWorld;
}

const Mat4& Transform::GetWorldToParent() const
{
    if( HasParent() )
        return m_parent->GetWorldToLocal();
    else
        return Mat4::IDENTITY;
}

const Mat4& Transform::GetParentToWorld() const
{
    if( HasParent() )
        return m_parent->GetLocalToWorld();
    else
        return Mat4::IDENTITY;
}

const Mat4& Transform::GetWorldToLocal() const
{
    if( m_worldToLocalDirty )
    {
        m_worldToLocal = GetLocalToWorld().Inverse();
        SetWorldToLocalDirty( false );
    }
    return m_worldToLocal;
}

const Mat4& Transform::GetParentToLocal() const
{
    if( m_parentToLocalDirty )
    {
        m_parentToLocal = GetLocalToParent().Inverse();
        SetParentToLocalDirty( false );
    }
    return m_parentToLocal;
}

Vec3 Transform::GetLocalPosition() const
{
    return m_localPosition;
}

void Transform::SetLocalPosition( const Vec3& position )
{
    m_localPosition = position;
    SetLocalToParentDirty( true );
}

void Transform::TranslateLocal( const Vec3& translation )
{
    Vec3 tranInParentSpace = GetLocalToParent() * translation;
    SetLocalPosition( m_localPosition + tranInParentSpace );
}

Vec3 Transform::GetWorldPosition() const
{
    return Vec3( GetLocalToWorld().T );
}

void Transform::SetWorldPosition( const Vec3& position )
{
    if( !HasParent() )
    {
        SetLocalPosition( position );
        return;
    }
    Vec3 positionInParentSpace = Vec3( GetWorldToParent() * Vec4( position, 1 ) );
    SetLocalPosition( positionInParentSpace );
}

void Transform::TranslateWorld( const Vec3& translation )
{
    if( !HasParent() )
    {
        TranslateLocal( translation );
        return;
    }
    Vec3 translationInParentSpace = GetWorldToParent() * translation;
    TranslateLocal( translationInParentSpace );
}

Vec3 Transform::GetLocalEuler() const
{
    return m_localEuler;
}

void Transform::SetLocalEuler( const Vec3& euler )
{
    m_localEuler = euler;
    SetLocalToParentDirty( true );
}

void Transform::RotateLocalEuler( const Vec3& euler )
{
    Mat4 rot = Mat4::MakeRotationEuler( euler );
    SetLocalToParent( GetLocalToParent() * rot );

    //     m_localEuler += euler;
    //     //m_localEuler.x = Clampf( m_localEuler.x, -90, 90 );
    //     m_localEuler.y = MapDegreesToPlusMinus180( m_localEuler.y );
    //     m_localEuler.z = MapDegreesToPlusMinus180( m_localEuler.z );

    SetLocalToParentDirty( true );
}


Vec3 Transform::GetWorldEuler() const
{
    RegenWorldSRTIfDirty();
    return m_worldEuler;
}

void Transform::SetWorldEuler( const Vec3& euler )
{
    if( !HasParent() )
    {
        SetLocalEuler( euler );
        return;
    }
    Vec3 parentEuler = m_parent->GetWorldEuler();
    SetLocalEuler( euler - parentEuler );
}

void Transform::RotateWorldEuler( const Vec3& euler )
{
    Vec3 newWorlEuler = GetWorldEuler() + euler;
    SetWorldEuler( newWorlEuler );
}

Vec3 Transform::GetLocalScale() const
{
    return m_localScale;
}

void Transform::SetLocalScale( const Vec3& scale )
{
    m_localScale = scale;
    SetLocalToParentDirty( true );
}

void Transform::SetLocalScale( float uniformScale )
{
    SetLocalScale( uniformScale * Vec3::ONES );
}

Vec3 Transform::GetWorldScale() const
{
    RegenWorldSRTIfDirty();
    return m_worldScale;
}

void Transform::LookAt( const Vec3& targetWorld, const Vec3& worldUp /*= Vec3::UP */ )
{
    // calculations are done in parent space
    Vec3 position = GetLocalPosition();
    Vec3 target = Vec3( GetWorldToParent() * Vec4( targetWorld, 1 ) );
    Vec3 upHint = GetWorldToParent() * worldUp;

    //Get Forward
    Vec3 forward = target - position;
    forward.NormalizeAndGetLength();
    Vec3 right;

    //Get Right
    right = Cross( upHint, forward );
    if( right == Vec3::ZEROS )
        right = Vec3::RIGHT;
    right.NormalizeAndGetLength();

    //Get Up
    Vec3 up = Cross( forward, right );

    //Rotation
    Mat4 localRot;
    localRot.I = Vec4( right );
    localRot.J = Vec4( up );
    localRot.K = Vec4( forward );

    //Translation
    Mat4 localTrans;
    localTrans.T = Vec4( position, 1 );

    SetLocalToParent( localTrans * localRot );

    //rotation and translation have fast inverses
    Mat4 parentToLocalRot = localRot;
    parentToLocalRot.InvertRotation();
    Mat4 parentToLocalTran = localTrans;
    parentToLocalTran.InvertTranslation();

    m_parentToLocal = parentToLocalRot * parentToLocalTran;
    SetParentToLocalDirty( false );
}

void Transform::FaceCamera( const Camera* cam )
{
    Mat4 targetRot = cam->GetCamToWorldMatrix();

    targetRot.K = -targetRot.K;

    Vec3 euler = targetRot.DecomposeEuler();

    SetWorldEuler( euler );
}

bool Transform::HasParent() const
{
    return m_parent != nullptr;
}

void Transform::SetParent( Transform* parent )
{
    // Save world rotation and position
    Vec3 worldEuler = GetWorldEuler();
    Vec3 worldPosition = GetWorldPosition();
    // Remove from old parent
    if( m_parent )
        m_parent->RemoveChild( this );

    // Set world rotation and position back
    m_parent = parent;
    SetWorldEuler( worldEuler );
    SetWorldPosition( worldPosition );

    // Add to new parent
    if( parent )
        parent->AddChild( this );
}

void Transform::SetChildrenLocalToWorldDirty( bool dirty ) const
{
    for( auto& child : m_children )
    {
        child->SetLocalToWorldDirty( dirty );
    }
}

void Transform::SetLocalToParentDirty( bool dirty ) const
{
    m_localToParentDirty = dirty;
    if( dirty )
    {
        SetLocalToWorldDirty( true );
        SetParentToLocalDirty( true );
    }
}

void Transform::SetLocalToWorldDirty( bool dirty ) const
{
    m_localToWorldDirty = true;
    if( dirty )
    {
        SetWorldToLocalDirty( true );
        SetLocalToWorldSRTDirty( true );
        SetChildrenLocalToWorldDirty( true );
    }
}

void Transform::SetWorldToLocalDirty( bool dirty ) const
{
    m_worldToLocalDirty = dirty;
}

void Transform::SetParentToLocalDirty( bool dirty ) const
{
    m_parentToLocalDirty = dirty;
}

void Transform::SetLocalToWorldSRTDirty( bool dirty ) const
{
    m_worldSRTDirty = dirty;
}

void Transform::RegenLocalToWorldIfDirty() const
{
    if( m_localToWorldDirty )
    {
        if( HasParent() )
            m_localToWorld = GetParentToWorld() * GetLocalToParent();
        else
            m_localToWorld = GetLocalToParent();
        SetLocalToWorldDirty( false );
    }
}

void Transform::RegenLocalToParentIfDirty() const
{
    if( m_localToParentDirty )
    {
        m_localToParent = Mat4::MakeFromSRT( m_localScale, m_localEuler, m_localPosition );
        SetLocalToParentDirty( false );
    }
}

void Transform::RegenWorldSRTIfDirty() const
{
    if( m_worldSRTDirty )
    {
        m_localToWorld.DecomposeToSRT( m_worldScale, m_worldEuler, m_worldPosition );
        SetLocalToWorldSRTDirty( false );
    }
}

void Transform::AddChild( Transform* child )
{
    if( child )
        m_children.push_back( child );
}

void Transform::RemoveChild( Transform* child )
{
    if( child )
        ContainerUtils::EraseOneValue( m_children, child );
}
