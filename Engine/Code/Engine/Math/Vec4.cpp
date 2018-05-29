#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"




const Vec4 Vec4::ZEROS = Vec4( 0.f, 0.f, 0.f, 0.f );
const Vec4 Vec4::ZEROS_W_ONE = Vec4( 0.f, 0.f, 0.f, 1.f );
const Vec4 Vec4::ONES = Vec4( 1.f, 1.f, 1.f, 1.f );
const Vec4 Vec4::NEG_ONES = Vec4( -1.f, -1.f, -1.f, -1.f );



Vec4::Vec4( float initX /*= 0.f*/, float initY /*= 0.f*/, float initZ /*= 0.f*/, float initW /*= 0.f */ )
    : x( initX )
    , y( initY )
    , z( initZ )
    , w( initW )
{

}

Vec4::Vec4( const Vec3& vec3, float appendW )
    : x(vec3.x)
    , y(vec3.y)
    , z( vec3.z )
    , w( appendW )
{

}

Vec4::Vec4( const Vec2& vec2, float appendZ /*= 0.f*/, float appendW /*= 0.f */ )
    : x( vec2.x )
    , y( vec2.y )
    , z( appendZ )
    , w( appendW )
{

}

const Vec4 Vec4::operator+( const Vec4& vecToAdd ) const
{
    return Vec4( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z, w + vecToAdd.w );
}

const Vec4 Vec4::operator-( const Vec4& vecToSubtract ) const
{
    return Vec4( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w );
}

const Vec4 Vec4::operator-() const
{
    return Vec4( -x, -y, -z, -w );
}

void Vec4::operator-=( const Vec4& vecToSubtract )
{
    x -= vecToSubtract.x;
    y -= vecToSubtract.y;
    z -= vecToSubtract.z;
    w -= vecToSubtract.w;
}

void Vec4::operator+=( const Vec4& vecToAdd )
{
    x += vecToAdd.x;
    y += vecToAdd.y;
    z += vecToAdd.z;
    w += vecToAdd.w;
}

void Vec4::operator*=( float uniformScale )
{
    x *= uniformScale;
    y *= uniformScale;
    z *= uniformScale;
    z *= uniformScale;
}

const Vec4 Vec4::operator*( float uniformScale ) const
{
    return Vec4( x * uniformScale, y * uniformScale, z * uniformScale, w * uniformScale );
}

Vec3 Vec4::XYZ() const
{
    return Vec3( x, y, z );
}

const Vec4 operator*( float uniformScale, const Vec4& vecToScale )
{
    return vecToScale * uniformScale;
}

void Vec4::operator=( const Vec4& copyFrom )
{
    x = copyFrom.x;
    y = copyFrom.y;
    z = copyFrom.z;
    w = copyFrom.w;
}

void Vec4::operator/=( float uniformDivisor )
{
    x /= uniformDivisor;
    y /= uniformDivisor;
    z /= uniformDivisor;
    w /= uniformDivisor;
}

const Vec4 Vec4::operator/( float inverseScale ) const
{
    return Vec4( x / inverseScale, y / inverseScale, z / inverseScale, w / inverseScale );
}

bool Vec4::operator==( const Vec4& compare ) const
{
    return ( x == compare.x ) && ( y == compare.y ) && ( z == compare.z ) && ( w == compare.w );
}

bool Vec4::operator!=( const Vec4& compare ) const
{
    return !( *this == compare );
}

float Dot( const Vec4& vecA, const Vec4& vecB )
{
    return ( vecA.x * vecB.x ) + ( vecA.y * vecB.y ) + ( vecA.z * vecB.z ) + ( vecA.w * vecB.w );
}
