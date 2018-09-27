#include <cmath>

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Profiler/Profiler.hpp"

// static values
const Vec2 Vec2::ONES       = Vec2( 1.f, 1.f );
const Vec2 Vec2::ZEROS      = Vec2( 0.f, 0.f );
const Vec2 Vec2::ONE_ZERO   = Vec2( 1.f, 0.f );
const Vec2 Vec2::ZERO_ONE   = Vec2( 0.f, 1.f );
const Vec2 Vec2::NEG_ONES   = Vec2( -1.f, -1.f );
const Vec2 Vec2::POINT_FIVES= Vec2( 0.5f, 0.5f );


const Vec2 Vec2::NORTH      = Vec2( 0.f, 1.f );
const Vec2 Vec2::SOUTH      = Vec2( 0.f, -1.f );
const Vec2 Vec2::EAST       = Vec2( 1.f, 0.f );
const Vec2 Vec2::WEST       = Vec2( -1.f, 0.f );
const Vec2 Vec2::NORTH_EAST = Vec2( M_SQRT2_OVER_2, M_SQRT2_OVER_2 );
const Vec2 Vec2::NORTH_WEST = Vec2( -M_SQRT2_OVER_2, M_SQRT2_OVER_2 );
const Vec2 Vec2::SOUTH_EAST = Vec2( M_SQRT2_OVER_2, -M_SQRT2_OVER_2 );
const Vec2 Vec2::SOUTH_WEST = Vec2( -M_SQRT2_OVER_2, -M_SQRT2_OVER_2 );


Vec2::Vec2( const Vec2& copyFrom )
    : x( copyFrom.x )
    , y( copyFrom.y )
{}

Vec2::Vec2( float initialX, float initialY )
    : x( initialX )
    , y( initialY )
{
}

Vec2::Vec2( const Vec3& vec3 )
    : x( vec3.x )
    , y( vec3.y )
{}

Vec2::Vec2( const Vec4& vec4 )
    : x( vec4.x )
    , y( vec4.y )
{}

Vec2::Vec2( const IVec2& ivec2 )
    : x( (float) ivec2.x )
    , y( (float) ivec2.y )
{

}

const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
    return Vec2( x + vecToAdd.x, y + vecToAdd.y );
}

const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
    return Vec2( x - vecToSubtract.x, y - vecToSubtract.y );
}


const Vec2 Vec2::operator*( float uniformScale ) const
{
    return Vec2( x * uniformScale, y * uniformScale );
}

const Vec2 Vec2::operator*( const Vec2& vec2 ) const
{
    return Vec2( x * vec2.x, y * vec2.y );
}

const Vec2 Vec2::operator/( float inverseScale ) const
{
    return Vec2( x / inverseScale, y / inverseScale );
}

const Vec2 Vec2::operator/( const Vec2& vec2 ) const
{
    return Vec2( x / vec2.x, y / vec2.y );
}

const Vec2 Vec2::operator-() const
{
    return Vec2( -x, -y );
}

void Vec2::operator+=( const Vec2& vecToAdd )
{
    x += vecToAdd.x;
    y += vecToAdd.y;
}

void Vec2::operator-=( const Vec2& vecToSubtract )
{
    x -= vecToSubtract.x;
    y -= vecToSubtract.y;
}

void Vec2::operator*=( const float uniformScale )
{
    x *= uniformScale;
    y *= uniformScale;
}

void Vec2::operator/=( const float uniformDivisor )
{
    x /= uniformDivisor;
    y /= uniformDivisor;
}

void Vec2::operator=( const Vec2& copyFrom )
{
    x = copyFrom.x;
    y = copyFrom.y;
}

const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
    return Vec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );
}


bool Vec2::operator==( const Vec2& compare ) const
{
    return ( x == compare.x ) && ( y == compare.y );
}

bool Vec2::operator!=( const Vec2& compare ) const
{
    return !( *this == compare );
}

float Vec2::GetLength() const
{
    return sqrtf( ( x * x ) + ( y * y ) );
}

float Vec2::GetLengthSquared() const
{
    return ( x * x ) + ( y * y );
}

float Vec2::NormalizeAndGetLength()
{
    float length = GetLength();
    if( !AlmostZero( length ) )
    {
        x = x / length;
        y = y / length;
    }
    return length;
}

void Vec2::Normalize()
{
    NormalizeAndGetLength();
}

Vec2 Vec2::GetNormalized() const
{
    float length = GetLength();
    if( length != 0 )
    {
        return Vec2( x / length, y / length );
    }
    else
    {
        return Vec2( x, y );
    }
}

Vec2 Vec2::GetOrthogonal() const
{
    return Vec2( -y, x );
}

Vec2 Vec2::GetClosestCardinalDir()
{
    float dotNorth = Dot( *this, NORTH );
    float dotSouth = -dotNorth;
    float dotEast = Dot( *this, EAST );
    float dotWest = -dotEast;

    Vec2 bestDir = NORTH;
    float largestDot = dotNorth;

    if( dotSouth > largestDot )
    {
        largestDot = dotSouth;
        bestDir = SOUTH;
    }

    if( dotEast > largestDot )
    {
        largestDot = dotEast;
        bestDir = EAST;
    }

    if( dotWest > largestDot )
    {
        largestDot = dotWest;
        bestDir = WEST;
    }
    return bestDir;
}

float Vec2::GetOrientationDegrees() const
{
    return Atan2Deg( y, x );
}

float Vec2::GetValueOnAxis( Axis axis ) const
{
    switch( axis )
    {
    case Axis::X:
        return x;
    case Axis::Y:
        return y;
    default:
        return 0.f;
    }
}

Vec3 Vec2::ToVec3()
{
    return Vec3( *this );
}

Vec3 Vec2::ToVec3XZ()
{
    return Vec3::MakeFromXZ( *this );
}

Vec2 Vec2::MakeVectorOnAxis( Axis axis, float lengthOnAxis )
{
    float x = 0.f;
    float y = 0.f;
    switch( axis )
    {
    case Axis::X:
        x = lengthOnAxis;
        break;

    case Axis::Y:
        y = lengthOnAxis;
        break;

    default:
        break;
    }
    return Vec2( x, y );
}

Vec2 Vec2::MakeFromXZ( const Vec3& vec3 )
{
    return Vec2( vec3.x, vec3.z );
}

Vec2 Vec2::MakeDirectionAtDegrees( float degrees )
{
    return Vec2( CosDeg( degrees ), SinDeg( degrees ) );
}

bool Vec2::IsPointLeftOfLine( const Vec2& lineStart, const Vec2& lineEnd, const Vec2& point )
{
    float cross = Cross( lineEnd - lineStart, point - lineStart );
    return cross > 0;

}

float Vec2::GetDistance( const Vec2 & a, const Vec2 & b )
{
    return ( a - b ).GetLength();
}

float Vec2::GetDistanceSquared( const Vec2 & a, const Vec2 & b )
{
    return ( a - b ).GetLengthSquared();
}

Vec2 Vec2::GetProjectedVector( const Vec2& vectorToProject, const Vec2& projectOnto )
{
    float dot = Dot( projectOnto, vectorToProject );
    return projectOnto * ( dot / projectOnto.GetLengthSquared() );
}

Vec2 Vec2::NearestPointOnLine( const Vec2 & point, const Vec2 & lineA, const Vec2 & lineB )
{
    Vec2 displacementToEnd = lineB - lineA;
    Vec2 displacementToPoint = point - lineA;
    return lineA + GetProjectedVector( displacementToPoint, displacementToEnd );
}

// math from: https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
//TODO untested, test against NearestPointOnLine
float Vec2::PointToLineDist( const Vec2 & point, const Vec2 & lineA, const Vec2 & lineB )
{
    float px = point.x;
    float py = point.y;
    float ax = lineA.x;
    float ay = lineA.y;
    float bx = lineB.x;
    float by = lineB.y;
    float numerator = fabsf( ( by - ay ) * px - ( bx - ax ) * py + bx * ay - by * ax );
    float denominator = sqrtf( ( by - ay ) * ( by - ay ) + ( bx - ax ) * ( bx - ax ) );
    return numerator / denominator;
}

void Vec2::RotateDegrees( float degrees )
{
    float cosDeg = CosDeg( degrees );
    float sinDeg = SinDeg( degrees );
    float oldX = x;
    x = x * cosDeg - y * sinDeg;
    y = oldX * sinDeg + y * cosDeg;
}

ParseStatus Vec2::SetFromText( const string& text, const string& delimiter )
{
    vector<float> out_floats;
    ParseStatus status = StringUtils::ParseToFloats( text, out_floats, delimiter );

    if( out_floats.size() < 2 )
        return PARSE_WRONG_NUMBER_OF_TOKENS;

    x = out_floats[0];
    y = out_floats[1];

    return status;
}

void Vec2::Scale( const Vec2& scale )
{
    x *= scale.x;
    y *= scale.y;
}

bool Vec2::SnapToZero()
{
    x = ::SnapToZero( x );
    y = ::SnapToZero( y );
    return x == 0.f && y == 0.f;
}

Vec2 Vec2::Reflect( const Vec2& vectorToReflect, const Vec2& reflectionNormal )
{
    float valueOnNormal = Dot( reflectionNormal, vectorToReflect );
    return vectorToReflect - ( 2.f * valueOnNormal * reflectionNormal );
}

const Vec2 Vec2::GetTransformedIntoBasis( const Vec2& originalVector, const Vec2& newBasisI, const Vec2& newBasisJ )
{
    float i = Dot( originalVector, newBasisI );
    float j = Dot( originalVector, newBasisJ );
    return Vec2( i, j );
}

const Vec2 Vec2::GetTransformedOutOfBasis( const Vec2& vectorInBasis, const Vec2& oldBasisI, const Vec2& oldBasisJ )
{
    float i = vectorInBasis.x;
    float j = vectorInBasis.y;
    return ( i * oldBasisI ) + ( j * oldBasisJ );
}

void Vec2::DecomposeVectorIntoBasis( const Vec2& originalVector, const Vec2& newBasisI, const Vec2& newBasisJ, Vec2& out_vectorAlongI, Vec2& out_vectorAlongJ )
{
    float i = Dot( originalVector, newBasisI );
    float j = Dot( originalVector, newBasisJ );
    out_vectorAlongI = i * newBasisI;
    out_vectorAlongJ = j * newBasisJ;
}

float Dot( const Vec2 & v1, const Vec2 & v2 )
{
    return ( v1.x * v2.x ) + ( v1.y * v2.y );
}

float Cross( const Vec2 & v1, const Vec2 & v2 )
{
    return ( v1.x * v2.y ) - ( v1.y * v2.x );
}

Vec2 Slerp( const Vec2& vecA, const Vec2& vecB, float t )
{
    float aLen = vecA.GetLength();
    float bLen = vecB.GetLength();

    float len = Lerp( aLen, bLen, t );
    Vec2 unit = SlerpUnit( vecA / aLen, vecB / bLen, t );
    return len * unit;
}

Vec2 SlerpUnit( const Vec2& vecA, const Vec2& vecB, float t )
{
    float cosAngle = Clampf( Dot( vecA, vecB ), -1.0f, 1.0f );
    float angle = acosf( cosAngle );
    if( angle < EPSILON )
    {
        return Lerp( vecA, vecB, t );
    }
    else
    {
        float posNum = sinf( t * angle );
        float negNum = sinf( ( 1.0f - t ) * angle );
        float den = sinf( angle );

        return ( negNum / den ) * vecA + ( posNum / den ) * vecB;
    }
}

Vec2 TurnToward( const Vec2& current, const Vec2& goal, float maxTurnDeg )
{
    float currentDeg = current.GetOrientationDegrees();
    float goalDeg = goal.GetOrientationDegrees();

    float newDeg = ::TurnToward( currentDeg, goalDeg, maxTurnDeg );

    return Vec2::MakeDirectionAtDegrees( newDeg );
}

Vec2 Min( const Vec2& a, const Vec2& b )
{
    return Vec2( Minf( a.x, b.y ), Minf( a.y, b.y ) );
}

Vec2 Max( const Vec2& a, const Vec2& b )
{
    return Vec2( Maxf( a.x, b.y ), Maxf( a.y, b.y ) );
}
