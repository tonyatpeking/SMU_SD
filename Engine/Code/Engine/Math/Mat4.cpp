#include <algorithm>
#include "Engine/Math/Mat4.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Plane.hpp"
#include "Engine/Core/StringUtils.hpp"

const Mat4 Mat4::IDENTITY = Mat4();



Mat4::Mat4( const float* sixteenValuesBasisMajor )
{
    std::memcpy( this, sixteenValuesBasisMajor, sizeof( float ) * 16 );
}

Mat4::Mat4( const Vec2& iBasis, const Vec2& jBasis, const Vec2& translation /*= Vec2( 0.f, 0.f ) */ )
    : Ix( iBasis.x )
    , Iy( iBasis.y )
    , Jx( jBasis.x )
    , Jy( jBasis.y )
    , Tx( translation.x )
    , Ty( translation.y )
{
}


Mat4::Mat4( const Vec3& i, const Vec3& j, const Vec3& k, const Vec3& t )
    : I( i )
    , J( j )
    , K( k )
    , T( t, 1.f )
{

}

Mat4 Mat4::MakeTranslation( const Vec3& translation )
{
    Mat4 mat;
    mat.Tx = translation.x;
    mat.Ty = translation.y;
    mat.Tz = translation.z;
    return mat;
}

Mat4 Mat4::MakeTranslation( float x, float y, float z )
{
    return MakeTranslation( Vec3( x, y, z ) );
}

Mat4 Mat4::MakeRotationEuler( float yawDeg, float pitchDeg, float rollDeg )
{
    //  yaw - y          pitch - x        roll - z
    //  c  0  s  0       1  0  0  0       c -s  0  0
    //  0  1  0  0   *   0  c -s  0   *   s  c  0  0
    // -s  0  c  0       0  s  c  0       0  0  1  0
    //  0  0  0  1       0  0  0  1       0  0  0  1
    Mat4 mat;
    float c1 = CosDeg( yawDeg );
    float s1 = SinDeg( yawDeg );
    float c2 = CosDeg( pitchDeg );
    float s2 = SinDeg( pitchDeg );
    float c3 = CosDeg( rollDeg );
    float s3 = SinDeg( rollDeg );

    mat.Ix = ( c1 * c3 ) + ( s1 * s2 * s3 );
    mat.Jx = ( s1 * s2 * c3 ) - ( c1 * s3 );
    mat.Kx = s1 * c2;
    mat.Iy = c2 * s3;
    mat.Jy = c2 * c3;
    mat.Ky = -s2;
    mat.Iz = ( c1 * s2 * s3 ) - ( s1 * c3 );
    mat.Jz = ( s1 * s3 ) + ( c1 * s2 * c3 );
    mat.Kz = c1 * c2;

    return mat;
}

Mat4 Mat4::MakeRotationEuler( Vec3 euler )
{
    return MakeRotationEuler( euler.y, euler.x, euler.z );
}

void Mat4::operator=( const Mat4& copyFrom )
{
    std::memcpy( this, &copyFrom, sizeof( float ) * 16 );
}

const Mat4 Mat4::operator*( const Mat4& rhs )  const
{
    //  Ix Jx Kx Tx     Ix Jx Kx Tx
    //  Iy Jy Ky Ty  *  Iy Jy Ky Ty
    //  Iz Jz Kz Tz     Iz Jz Kz Tz
    //  Iw Jw Kw Tw     Iw Jw Kw Tw

    Mat4 result;

    result.Ix = ( Ix * rhs.Ix ) + ( Jx * rhs.Iy ) + ( Kx * rhs.Iz ) + ( Tx * rhs.Iw );
    result.Iy = ( Iy * rhs.Ix ) + ( Jy * rhs.Iy ) + ( Ky * rhs.Iz ) + ( Ty * rhs.Iw );
    result.Iz = ( Iz * rhs.Ix ) + ( Jz * rhs.Iy ) + ( Kz * rhs.Iz ) + ( Tz * rhs.Iw );
    result.Iw = ( Iw * rhs.Ix ) + ( Jw * rhs.Iy ) + ( Kw * rhs.Iz ) + ( Tw * rhs.Iw );

    result.Jx = ( Ix * rhs.Jx ) + ( Jx * rhs.Jy ) + ( Kx * rhs.Jz ) + ( Tx * rhs.Jw );
    result.Jy = ( Iy * rhs.Jx ) + ( Jy * rhs.Jy ) + ( Ky * rhs.Jz ) + ( Ty * rhs.Jw );
    result.Jz = ( Iz * rhs.Jx ) + ( Jz * rhs.Jy ) + ( Kz * rhs.Jz ) + ( Tz * rhs.Jw );
    result.Jw = ( Iw * rhs.Jx ) + ( Jw * rhs.Jy ) + ( Kw * rhs.Jz ) + ( Tw * rhs.Jw );

    result.Kx = ( Ix * rhs.Kx ) + ( Jx * rhs.Ky ) + ( Kx * rhs.Kz ) + ( Tx * rhs.Kw );
    result.Ky = ( Iy * rhs.Kx ) + ( Jy * rhs.Ky ) + ( Ky * rhs.Kz ) + ( Ty * rhs.Kw );
    result.Kz = ( Iz * rhs.Kx ) + ( Jz * rhs.Ky ) + ( Kz * rhs.Kz ) + ( Tz * rhs.Kw );
    result.Kw = ( Iw * rhs.Kx ) + ( Jw * rhs.Ky ) + ( Kw * rhs.Kz ) + ( Tw * rhs.Kw );

    result.Tx = ( Ix * rhs.Tx ) + ( Jx * rhs.Ty ) + ( Kx * rhs.Tz ) + ( Tx * rhs.Tw );
    result.Ty = ( Iy * rhs.Tx ) + ( Jy * rhs.Ty ) + ( Ky * rhs.Tz ) + ( Ty * rhs.Tw );
    result.Tz = ( Iz * rhs.Tx ) + ( Jz * rhs.Ty ) + ( Kz * rhs.Tz ) + ( Tz * rhs.Tw );
    result.Tw = ( Iw * rhs.Tx ) + ( Jw * rhs.Ty ) + ( Kw * rhs.Tz ) + ( Tw * rhs.Tw );

    return result;
}

const Vec3 Mat4::operator*( const Vec3& rhs ) const
{
    Vec4 vec4 = Vec4( rhs );
    return Vec3( ( *this ) * vec4 );
}

const Vec4 Mat4::operator*( const Vec4& rhs ) const
{
    //  Ix Jx Kx Tx     x
    //  Iy Jy Ky Ty  *  y
    //  Iz Jz Kz Tz     z
    //  Iw Jw Kw Tw     w
    Vec4 result;
    result.x = ( Ix * rhs.x ) + ( Jx * rhs.y ) + ( Kx * rhs.z ) + ( Tx * rhs.w );
    result.y = ( Iy * rhs.x ) + ( Jy * rhs.y ) + ( Ky * rhs.z ) + ( Ty * rhs.w );
    result.z = ( Iz * rhs.x ) + ( Jz * rhs.y ) + ( Kz * rhs.z ) + ( Tz * rhs.w );
    result.w = ( Iw * rhs.x ) + ( Jw * rhs.y ) + ( Kw * rhs.z ) + ( Tw * rhs.w );
    return result;
}


bool Mat4::operator==( const Mat4& rhs ) const
{
    return I == rhs.I && J == rhs.J && K == rhs.K && T == rhs.T;
}

bool Mat4::operator!=( const Mat4& rhs ) const
{
    return !( *this == rhs );
}

bool Mat4::Invert()
{

    float inv[16], det;
    int i;

    inv[0] =
        el[5] * el[10] * el[15] -
        el[5] * el[11] * el[14] -
        el[9] * el[6] * el[15] +
        el[9] * el[7] * el[14] +
        el[13] * el[6] * el[11] -
        el[13] * el[7] * el[10];

    inv[4] =
        -el[4] * el[10] * el[15] +
        el[4] * el[11] * el[14] +
        el[8] * el[6] * el[15] -
        el[8] * el[7] * el[14] -
        el[12] * el[6] * el[11] +
        el[12] * el[7] * el[10];

    inv[8] =
        el[4] * el[9] * el[15] -
        el[4] * el[11] * el[13] -
        el[8] * el[5] * el[15] +
        el[8] * el[7] * el[13] +
        el[12] * el[5] * el[11] -
        el[12] * el[7] * el[9];

    inv[12] =
        -el[4] * el[9] * el[14] +
        el[4] * el[10] * el[13] +
        el[8] * el[5] * el[14] -
        el[8] * el[6] * el[13] -
        el[12] * el[5] * el[10] +
        el[12] * el[6] * el[9];

    inv[1] =
        -el[1] * el[10] * el[15] +
        el[1] * el[11] * el[14] +
        el[9] * el[2] * el[15] -
        el[9] * el[3] * el[14] -
        el[13] * el[2] * el[11] +
        el[13] * el[3] * el[10];

    inv[5] =
        el[0] * el[10] * el[15] -
        el[0] * el[11] * el[14] -
        el[8] * el[2] * el[15] +
        el[8] * el[3] * el[14] +
        el[12] * el[2] * el[11] -
        el[12] * el[3] * el[10];

    inv[9] =
        -el[0] * el[9] * el[15] +
        el[0] * el[11] * el[13] +
        el[8] * el[1] * el[15] -
        el[8] * el[3] * el[13] -
        el[12] * el[1] * el[11] +
        el[12] * el[3] * el[9];

    inv[13] =
        el[0] * el[9] * el[14] -
        el[0] * el[10] * el[13] -
        el[8] * el[1] * el[14] +
        el[8] * el[2] * el[13] +
        el[12] * el[1] * el[10] -
        el[12] * el[2] * el[9];

    inv[2] =
        el[1] * el[6] * el[15] -
        el[1] * el[7] * el[14] -
        el[5] * el[2] * el[15] +
        el[5] * el[3] * el[14] +
        el[13] * el[2] * el[7] -
        el[13] * el[3] * el[6];

    inv[6] =
        -el[0] * el[6] * el[15] +
        el[0] * el[7] * el[14] +
        el[4] * el[2] * el[15] -
        el[4] * el[3] * el[14] -
        el[12] * el[2] * el[7] +
        el[12] * el[3] * el[6];

    inv[10] =
        el[0] * el[5] * el[15] -
        el[0] * el[7] * el[13] -
        el[4] * el[1] * el[15] +
        el[4] * el[3] * el[13] +
        el[12] * el[1] * el[7] -
        el[12] * el[3] * el[5];

    inv[14] =
        -el[0] * el[5] * el[14] +
        el[0] * el[6] * el[13] +
        el[4] * el[1] * el[14] -
        el[4] * el[2] * el[13] -
        el[12] * el[1] * el[6] +
        el[12] * el[2] * el[5];

    inv[3] =
        -el[1] * el[6] * el[11] +
        el[1] * el[7] * el[10] +
        el[5] * el[2] * el[11] -
        el[5] * el[3] * el[10] -
        el[9] * el[2] * el[7] +
        el[9] * el[3] * el[6];

    inv[7] =
        el[0] * el[6] * el[11] -
        el[0] * el[7] * el[10] -
        el[4] * el[2] * el[11] +
        el[4] * el[3] * el[10] +
        el[8] * el[2] * el[7] -
        el[8] * el[3] * el[6];

    inv[11] =
        -el[0] * el[5] * el[11] +
        el[0] * el[7] * el[9] +
        el[4] * el[1] * el[11] -
        el[4] * el[3] * el[9] -
        el[8] * el[1] * el[7] +
        el[8] * el[3] * el[5];

    inv[15] =
        el[0] * el[5] * el[10] -
        el[0] * el[6] * el[9] -
        el[4] * el[1] * el[10] +
        el[4] * el[2] * el[9] +
        el[8] * el[1] * el[6] -
        el[8] * el[2] * el[5];

    det = el[0] * inv[0] + el[1] * inv[4] + el[2] * inv[8] + el[3] * inv[12];

    if( det == 0 )
        return false;

    det = 1.0f / det;

    for( i = 0; i < 16; i++ )
        el[i] = inv[i] * det;

    return true;

}

void Mat4::Transpose()
{
    Vec4 rowX = GetRowX();
    Vec4 rowY = GetRowY();
    Vec4 rowZ = GetRowZ();
    Vec4 rowW = GetRowW();
    I = rowX;
    J = rowY;
    K = rowZ;
    T = rowW;
}

Mat4 Mat4::Transposed()
{
    Mat4 transpose = *this;
    transpose.Transpose();
    return transpose;
}

Mat4 Mat4::LerpTransform( const Mat4& matA, const Mat4& matB, float t )
{
    Vec3 aRight = (Vec3) matA.I;
    Vec3 bRight = (Vec3) matB.I;
    Vec3 aUp = (Vec3) matA.J;
    Vec3 bUp = (Vec3) matB.J;
    Vec3 aForward = (Vec3) matA.K;
    Vec3 bForward = (Vec3) matB.K;
    Vec3 a_translation = (Vec3) matA.T;
    Vec3 b_translation = (Vec3) matB.T;

    Vec3 right = Slerp( aRight, bRight, t );
    Vec3 up = Slerp( aUp, bUp, t );
    Vec3 forward = Slerp( aForward, bForward, t );
    Vec3 translation = Lerp( a_translation, b_translation, t );

    return Mat4( right, up, forward, translation );
}

void Mat4::InvertTranslation()
{
    Tx = -Tx;
    Ty = -Ty;
    Tz = -Tz;
}

Mat4 Mat4::InverseTranslation()
{
    Mat4 mat4 = *this;
    mat4.InvertTranslation();
    return mat4;
}

void Mat4::InvertRotation()
{
    Transpose();
}


Mat4 Mat4::InverseRotation()
{
    return this->Transposed();
}

void Mat4::SnapToZero()
{
    for (int idx = 0; idx < 16 ; ++idx)
    {
        el[idx] = ::SnapToZero( el[idx] );
    }
}

Vec3 Mat4::TransformPosition( const Vec3& rhs ) const
{
    return Vec3( (*this) * Vec4( rhs, 1 ));
}

Vec3 Mat4::TransformDisplacement( const Vec3& rhs ) const
{
    return Vec3( ( *this ) * Vec4( rhs, 0 ) );
}

Plane Mat4::TransformPlane( const Plane& plane ) const
{
    Vec3 point = plane.GetPoint();
    Vec3 normal = plane.GetNormal();

    return Plane( TransformDisplacement( normal ), TransformPosition( point ) );
}

Vec2 Mat4::TransformPosition2D( const Vec2& position2D )
{
    Vec2 X2D = Vec2( Ix, Jx );
    Vec2 Y2D = Vec2( Iy, Jy );
    float newX = Dot( X2D, position2D ) + Tx;
    float newY = Dot( Y2D, position2D ) + Ty;
    return Vec2( newX, newY );

}

Vec2 Mat4::TransformDisplacement2D( const Vec2& displacement2D )
{
    Vec2 X2D = Vec2( Ix, Jx );
    Vec2 Y2D = Vec2( Iy, Jy );
    float newX = Dot( X2D, displacement2D );
    float newY = Dot( Y2D, displacement2D );
    return Vec2( newX, newY );

}

Vec4 Mat4::GetRowX() const
{
    return Vec4( Ix, Jx, Kx, Tx );
}

Vec4 Mat4::GetRowY() const
{
    return Vec4( Iy, Jy, Ky, Ty );
}

Vec4 Mat4::GetRowZ() const
{
    return Vec4( Iz, Jz, Kz, Tz );
}

Vec4 Mat4::GetRowW() const
{
    return Vec4( Iw, Jw, Kw, Tw );
}

void Mat4::DecomposeToSRT( Vec3& out_scale, Vec3& out_euler, Vec3& out_translation ) const
{
    out_translation = Vec3( T );
    out_scale = DecomposeScale();
    Mat4 rot = *this;
    if( 0 == out_scale.x || 0 == out_scale.y || 0 == out_scale.z )
    {
        LOG_WARNING( "could not decompose matrix to srt, scale was 0" );
        return;
    }
    rot.Scale( Vec3::ONES / out_scale );
    out_euler = rot.DecomposeEuler();
}

Mat4 Mat4::GetRotationalPart() const
{
    Vec3 scale = DecomposeScale();
    Mat4 rot = *this;
    if( 0 == scale.x || 0 == scale.y || 0 == scale.z )
    {
        LOG_WARNING( "could not get rotational part, scale was 0" );
        return rot;
    }
    rot.Scale( Vec3::ONES / scale );
    rot.T = Vec4( 0, 0, 0, 1 );
    return rot;
}

Vec3 Mat4::DecomposeEuler() const
{
    Vec3 euler;

    euler.x = -ArcSinDeg( Ky );


    if( Ky != 1 && Ky != -1 )
    {
        euler.z = Atan2Deg( Iy, Jy );
        euler.y = Atan2Deg( Kx, Kz );
    }
    // pitch is +-90, looking straight up or down, pretend roll is 0, and only calc yaw
    else
    {
        euler.z = 0;
        euler.y = Atan2Deg( Jx, Jz );
    }
    return euler;
}

Vec3 Mat4::DecomposeScale() const
{
    Vec3 scale;
    scale.x = Vec3( I ).GetLength();
    scale.y = Vec3( J ).GetLength();
    scale.z = Vec3( K ).GetLength();
    return scale;
}

bool Mat4::IsAnyInf() const
{
    for (int idx = 0; idx < 16 ; ++idx)
    {
        if( std::isinf( el[idx] ) )
            return true;
    }
    return false;
}

bool Mat4::IsAnyNaN() const
{
    for( int idx = 0; idx < 16; ++idx )
    {
        if( std::isnan( el[idx] ) )
            return true;
    }
    return false;
}

String Mat4::ToString() const
{
    return ::ToString( *this );
}

void Mat4::SetIdentity()
{
    *this = IDENTITY;
}

void Mat4::SetValues( const float* sixteenValuesBasisMajor )
{
    std::memcpy( this, sixteenValuesBasisMajor, sizeof( float ) * 16 );
}

void Mat4::SetRowX( Vec4 rowX )
{
    Ix = rowX.x;
    Jx = rowX.y;
    Kx = rowX.z;
    Tx = rowX.w;
}

void Mat4::SetRowY( Vec4 rowY )
{
    Iy = rowY.x;
    Jy = rowY.y;
    Ky = rowY.z;
    Ty = rowY.w;
}

void Mat4::SetRowZ( Vec4 rowZ )
{
    Iz = rowZ.x;
    Jz = rowZ.y;
    Kz = rowZ.z;
    Tz = rowZ.w;
}

void Mat4::SetRowW( Vec4 rowW )
{
    Iw = rowW.x;
    Jw = rowW.y;
    Kw = rowW.z;
    Tw = rowW.w;
}

void Mat4::RotateDegrees2D( float rotationDegreesAboutZ )
{
    //  Ix Jx Kx Tx     c -s  0  0
    //  Iy Jy Ky Ty  *  s  c  0  0
    //  Iz Jz Kz Tz     0  0  1  0
    //  Iw Jw Kw Tw     0  0  0  1

    float cos = CosDeg( rotationDegreesAboutZ );
    float sin = SinDeg( rotationDegreesAboutZ );
    float oldIx = Ix;
    float oldIy = Iy;
    float oldIz = Iz;
    float oldIw = Iw;

    float oldJx = Jx;
    float oldJy = Jy;

    // Not needed:
    //float oldJz = Jz;
    //float oldJw = Jw;

    Ix = oldIx * cos + oldJx * sin;
    Iy = oldIy * cos + oldJy * sin;
    Iz = Iz * cos + Jz * sin;
    Iw = Iw * cos + Jw * sin;

    Jx = oldIx * ( -sin ) + oldJx * cos;
    Jy = oldIy * ( -sin ) + oldJy * cos;
    Jz = oldIz * ( -sin ) + Jz * cos;
    Jw = oldIw * ( -sin ) + Jw * cos;
}

void Mat4::Translate2D( const Vec2& translation )
{
    //  Ix Jx Kx Tx     1  0  0  t.x
    //  Iy Jy Ky Ty  *  0  1  0  t.y
    //  Iz Jz Kz Tz     0  0  1  0
    //  Iw Jw Kw Tw     0  0  0  1

    // No need to save old values as none of the new values use changed values
    Tx = Ix * translation.x + Jx * translation.y + Tx;
    Ty = Iy * translation.x + Jy * translation.y + Ty;
    Tz = Iz * translation.x + Jz * translation.y + Tz;
    Tw = Iw * translation.x + Jw * translation.y + Tw;
}

void Mat4::ScaleUniform2D( float scaleXY )
{
    Scale2D( scaleXY, scaleXY );
}

void Mat4::Scale2D( float sx, float sy )
{
    //  Ix Jx Kx Tx     sx 0  0  0
    //  Iy Jy Ky Ty  *  0  sy 0  0
    //  Iz Jz Kz Tz     0  0  1  0
    //  Iw Jw Kw Tw     0  0  0  1

    // No need to save old values as none of the new values use changed values
    Ix = Ix * sx;
    Iy = Iy * sx;
    Iz = Iz * sx;
    Iw = Iw * sx;

    Jx = Jx * sy;
    Jy = Jy * sy;
    Jz = Jz * sy;
    Jw = Jw * sy;
}

void Mat4::Translate( const Vec3& t )
{
    //  Ix Jx Kx Tx     1  0  0  t.x
    //  Iy Jy Ky Ty  *  0  1  0  t.y
    //  Iz Jz Kz Tz     0  0  1  t.z
    //  Iw Jw Kw Tw     0  0  0  1

    // No need to save old values as none of the new values use changed values
    Tx = Ix * t.x + Jx * t.y + Kx * t.z + Tx;
    Ty = Iy * t.x + Jy * t.y + Ky * t.z + Ty;
    Tz = Iz * t.x + Jz * t.y + Kz * t.z + Tz;
    Tw = Iw * t.x + Jw * t.y + Kw * t.z + Tw;
}

void Mat4::Scale( const Vec3& s )
{
    //  Ix Jx Kx Tx     sx 0  0  0
    //  Iy Jy Ky Ty  *  0  sy 0  0
    //  Iz Jz Kz Tz     0  0  sz 0
    //  Iw Jw Kw Tw     0  0  0  1

    // No need to save old values as none of the new values use changed values
    Ix = Ix * s.x;
    Iy = Iy * s.x;
    Iz = Iz * s.x;
    Iw = Iw * s.x;

    Jx = Jx * s.y;
    Jy = Jy * s.y;
    Jz = Jz * s.y;
    Jw = Jw * s.y;

    Kx = Kx * s.z;
    Ky = Ky * s.z;
    Kz = Kz * s.z;
    Kw = Kw * s.z;
}

Mat4 Mat4::MakeFromSRT( const Vec3& scale, const Vec3& euler, const Vec3& translation )
{
    Mat4 srt = MakeRotationEuler( euler.y, euler.x, euler.z );
    srt.Scale( scale );
    srt.T = Vec4( translation, 1 );
    return srt;
}

Mat4 Mat4::MakeRotationDegrees2D( float rotationDegreesAboutZ )
{
    //    c -s  0  0
    //    s  c  0  0
    //    0  0  1  0
    //    0  0  0  1

    float cos = CosDeg( rotationDegreesAboutZ );
    float sin = SinDeg( rotationDegreesAboutZ );

    Mat4 rotMat = Mat4();
    rotMat.Ix = cos;
    rotMat.Iy = sin;
    rotMat.Jx = -sin;
    rotMat.Jy = cos;

    return rotMat;
}

Mat4 Mat4::MakeTranslation2D( const Vec2& translation )
{
    //    1  0  0  tx
    //    0  1  0  ty
    //    0  0  1  0
    //    0  0  0  1

    Mat4 transMat = Mat4();
    transMat.Tx = translation.x;
    transMat.Ty = translation.y;

    return transMat;
}

Mat4 Mat4::MakeScaleUniform2D( float scaleXY )
{
    return MakeScale2D( scaleXY, scaleXY );
}

Mat4 Mat4::MakeScale2D( float scaleX, float scaleY )
{
    //    sx 0  0  0
    //    0  sy 0  0
    //    0  0  1  0
    //    0  0  0  1

    Mat4 scaleMat = Mat4();
    scaleMat.Ix = scaleX;
    scaleMat.Jy = scaleY;

    return scaleMat;
}

Mat4 Mat4::MakeOrtho( const Vec2& bottomLeft, const Vec2& topRight, float near, float far )
{
    //    sx 0  0  0     1  0  0  tx
    //    0  sy 0  0  *  0  1  0  ty
    //    0  0  sz 0     0  0  1  tz
    //    0  0  0  1     0  0  0  1

    float scaleX = 2.f / ( topRight.x - bottomLeft.x );
    float scaleY = 2.f / ( topRight.y - bottomLeft.y );
    // NDC is left handed, we also use left handed in world/engine, so this is different from MSDN glOrtho
    // the z-depth of any point from [near,far] in ortho space is transformed to [-1,1] in clip space (-1=near, 1=far).
    float scaleZ = 2.f / ( far - near );

    float transX = -( topRight.x + bottomLeft.x ) / 2.f;
    float transY = -( topRight.y + bottomLeft.y ) / 2.f;
    float transZ = -( far + near ) / 2.f;

    Mat4 ortho = Mat4();

    ortho.Ix = scaleX;
    ortho.Jy = scaleY;
    ortho.Kz = scaleZ;

    ortho.Tx = transX * scaleX;
    ortho.Ty = transY * scaleY;
    ortho.Tz = transZ * scaleZ;

    return ortho;
}

Mat4 Mat4::MakeOrtho( float width, float height, float near, float far )
{
    Vec2 halfSize = Vec2( width, height ) * 0.5;
    return MakeOrtho( -halfSize, halfSize, near, far );
}

Mat4 Mat4::MakeProjection( float fovVertDeg, float aspect, float near, float far )
{
    Mat4 mat;

    float d = 1.0f / TanDeg( fovVertDeg / 2.f );
    float q = 1.0f / ( far - near );

    mat.Ix = d / aspect;
    mat.Jy = d;
    mat.Kz = ( near + far ) * q;
    mat.Kw = 1;
    mat.Tz = -2.0f * near * far * q;
    mat.Tw = 0;

    return mat;
}

Mat4 Mat4::Inverse() const
{
    Mat4 inv{};
    inv = *this;
    inv.Invert();
    return inv;
}

