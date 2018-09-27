#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommonH.hpp"

//static values

const Vec3 Vec3::UP = Vec3( 0.f, 1.f, 0.f );
const Vec3 Vec3::DOWN = Vec3( 0.f, -1.f, 0.f );
const Vec3 Vec3::LEFT = Vec3( -1.f, 0.f, 0.f );
const Vec3 Vec3::RIGHT = Vec3( 1.f, 0.f, 0.f );
const Vec3 Vec3::FORWARD = Vec3( 0.f, 0.f, 1.f );
const Vec3 Vec3::BACKWARD = Vec3( 0.f, 0.f, -1.f );

const Vec3 Vec3::ZEROS = Vec3( 0.f, 0.f, 0.f );
const Vec3 Vec3::ONES = Vec3( 1.f, 1.f, 1.f );
const Vec3 Vec3::NEG_ONES = Vec3( -1.f, -1.f, -1.f );


const Vec3 Vec3::POS_INFINITY = Vec3( INFINITY, INFINITY, INFINITY );
const Vec3 Vec3::NEG_INFINITY = Vec3( -INFINITY, -INFINITY, -INFINITY );

Vec3::Vec3( float initX /*= 0.f*/, float initY /*= 0.f*/, float initZ /*= 0.f*/ )
    : x( initX )
    , y( initY )
    , z( initZ )
{

}

Vec3::Vec3( const Vec2& vec2, float appendZ )
    : x( vec2.x )
    , y( vec2.y )
    , z( appendZ )
{

}

Vec3::Vec3( const Vec4& vec4 )
    : x( vec4.x )
    , y( vec4.y )
    , z( vec4.z )
{

}

Vec3 Vec3::MakeFromXZ( const Vec2& vec2, float y )
{
    return Vec3( vec2.x, y, vec2.y );
}

Vec2 Vec3::ToVec2() const
{
    return Vec2( *this );
}

Vec2 Vec3::ToVec2XZ() const
{
    return Vec2::MakeFromXZ( *this );
}

string Vec3::ToString() const
{
    return ::ToString( *this );
}

const Vec3 Vec3::operator+( const Vec3& vecToAdd ) const
{
    return Vec3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z );
}

const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const
{
    return Vec3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z );
}

const Vec3 Vec3::operator-() const
{
    return Vec3( -x, -y, -z );
}

const Vec3 Vec3::operator-=( const Vec3& vecToSubtract )
{
    x -= vecToSubtract.x;
    y -= vecToSubtract.y;
    z -= vecToSubtract.z;
    return *this;
}

const Vec3 Vec3::operator+=( const Vec3& vecToAdd )
{
    x += vecToAdd.x;
    y += vecToAdd.y;
    z += vecToAdd.z;
    return *this;
}

const Vec3 Vec3::operator*=( float uniformScale )
{
    x *= uniformScale;
    y *= uniformScale;
    z *= uniformScale;
    return *this;
}

const Vec3 operator*( float uniformScale, const Vec3& vecToScale )
{
    return vecToScale * uniformScale;
}

const Vec3 Vec3::operator*( float uniformScale ) const
{
    return Vec3( x * uniformScale, y * uniformScale, z * uniformScale );
}

const Vec3 Vec3::operator*( const Vec3& vec3 ) const
{
    return Vec3( x * vec3.x, y * vec3.y, z * vec3.z );
}

float Vec3::GetLength() const
{
    return sqrtf( ( x * x ) + ( y * y ) + ( z * z ) );
}

float Vec3::GetLengthSquared() const
{
    return ( x * x ) + ( y * y ) + ( z * z );
}

float Vec3::NormalizeAndGetLength()
{
    if( *this == ZEROS )
        return 0;

    float length = GetLength();
    x = x / length;
    y = y / length;
    z = z / length;
    return length;
}

Vec3 Vec3::GetNormalized() const
{
    float length = GetLength();
    if( length != 0 )
    {
        return Vec3( *this ) / length;
    }
    else
    {
        return Vec3( *this );
    }
}

Vec3 Vec3::GetClosestCardinalDir()
{
    float dotUp = Dot( *this, UP );
    float dotDown = -dotUp;
    float dotLeft = Dot( *this, LEFT );
    float dotRight = -dotLeft;
    float dotForward = Dot( *this, FORWARD );
    float dotBackward = -dotForward;

    Vec3 bestDir = UP;
    float largestDot = dotUp;

    if( dotDown > largestDot )
    {
        largestDot = dotDown;
        bestDir = DOWN;
    }

    if( dotLeft > largestDot )
    {
        largestDot = dotLeft;
        bestDir = LEFT;
    }

    if( dotRight > largestDot )
    {
        largestDot = dotRight;
        bestDir = RIGHT;
    }

    if( dotForward > largestDot )
    {
        largestDot = dotForward;
        bestDir = FORWARD;
    }

    if( dotBackward > largestDot )
    {
        largestDot = dotBackward;
        bestDir = BACKWARD;
    }
    return bestDir;
}

float Vec3::GetValueOnAxis( Axis axis ) const
{
    switch( axis )
    {
    case Axis::X:
        return x;
    case Axis::Y:
        return y;
    case Axis::Z:
        return z;
    default:
        return 0.f;
    }
}

void Vec3::SnapToZero()
{
    x = ::SnapToZero( x );
    y = ::SnapToZero( y );
    z = ::SnapToZero( z );
}

ParseStatus Vec3::SetFromText( const string& text, const string& delimiter )
{
    vector<float> out_floats;
    ParseStatus status = StringUtils::ParseToFloats( text, out_floats, delimiter );

    if( out_floats.size() != 3 )
        return PARSE_WRONG_NUMBER_OF_TOKENS;

    x = out_floats[0];
    y = out_floats[1];
    z = out_floats[2];

    return status;
}

Vec3 Vec3::Reflect( const Vec3& vectorToReflect, const Vec3& reflectionNormal )
{
    float valueOnNormal = Dot( reflectionNormal, vectorToReflect );
    return vectorToReflect - ( 2.f * valueOnNormal * reflectionNormal );
}

Vec3 Vec3::MakeVectorOnAxis( Axis axis, float lengthOnAxis/*=1.0f */ )
{
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
    switch( axis )
    {
    case Axis::X:
        x = lengthOnAxis;
        break;
    case Axis::Y:
        y = lengthOnAxis;
        break;
    case Axis::Z:
        z = lengthOnAxis;
        break;
    default:
        break;
    }
    return Vec3( x, y, z );
}

float Vec3::GetDistance( const Vec3& a, const Vec3& b )
{
    return ( a - b ).GetLength();
}

float Vec3::GetDistanceSquared( const Vec3& a, const Vec3& b )
{
    return ( a - b ).GetLengthSquared();
}

float Vec3::GetAngleBetween( const Vec3& a, const Vec3& b )
{
    float cosTheta = Dot( a, b ) / ( a.GetLength() * b.GetLength() );

    return ArcCosDeg( cosTheta );
}

Vec3 Vec3::NearestPointOnLine( const Vec3& point, const Vec3& lineStart, const Vec3& lineEnd )
{
    Vec3 displacementToEnd = lineEnd - lineStart;
    Vec3 displacementToPoint = point - lineStart;
    return lineStart + GetProjectedVector( displacementToPoint, displacementToEnd );
}

float Vec3::PointToLineDist( const Vec3& point, const Vec3& lineStart, const Vec3& lineEnd )
{
    return sqrtf( PointToLineDistSq( point, lineStart, lineEnd ) );
}

float Vec3::PointToLineDistSq( const Vec3& point, const Vec3& lineStart, const Vec3& lineEnd )
{
    float numerator = Cross( lineEnd - lineStart, lineStart - point ).GetLengthSquared();
    float denominator =( lineEnd - lineStart ).GetLengthSquared();
    return numerator / denominator;
}

Vec3 Vec3::GetProjectedVector( const Vec3& vectorToProject, const Vec3& projectOnto )
{
    float dot = Dot( projectOnto, vectorToProject );
    return projectOnto * ( dot / projectOnto.GetLengthSquared() );
}

Vec3 Vec3::GetProjectedToPlane( const Vec3& vectorToProject, const Vec3& planeNormal )
{
    return vectorToProject - GetProjectedVector( vectorToProject, planeNormal );
}


const Vec3 Vec3::GetTransformedIntoBasis( const Vec3& originalVector, const Vec3& newBasisI, const Vec3& newBasisJ, const Vec3& newBasisK )
{
    float i = Dot( originalVector, newBasisI );
    float j = Dot( originalVector, newBasisJ );
    float k = Dot( originalVector, newBasisK );
    return Vec3( i, j, k );
}

const Vec3 Vec3::GetTransformedOutOfBasis( const Vec3& vectorInBasis, const Vec3& oldBasisI, const Vec3& oldBasisJ, const Vec3& oldBasisK )
{
    float i = vectorInBasis.x;
    float j = vectorInBasis.y;
    float k = vectorInBasis.z;
    return ( i * oldBasisI ) + ( j * oldBasisJ ) + ( k * oldBasisK );
}

void Vec3::DecomposeVectorIntoBasis( const Vec3& originalVector, const Vec3& newBasisI, const Vec3& newBasisJ, const Vec3& newBasisK, Vec3& out_vectorAlongI, Vec3& out_vectorAlongJ, Vec3& out_vectorAlongK )
{
    float i = Dot( originalVector, newBasisI );
    float j = Dot( originalVector, newBasisJ );
    float k = Dot( originalVector, newBasisK );
    out_vectorAlongI = i * newBasisI;
    out_vectorAlongJ = j * newBasisJ;
    out_vectorAlongK = k * newBasisK;
}

const Vec3 Vec3::operator=( const Vec3& copyFrom )
{
    x = copyFrom.x;
    y = copyFrom.y;
    z = copyFrom.z;
    return *this;
}

const Vec3 Vec3::operator/=( float uniformDivisor )
{
    x /= uniformDivisor;
    y /= uniformDivisor;
    z /= uniformDivisor;
    return *this;
}

const Vec3 Vec3::operator/( float inverseScale ) const
{
    return Vec3( x / inverseScale, y / inverseScale, z / inverseScale );
}

const Vec3 Vec3::operator/( const Vec3& vec3 ) const
{
    return Vec3( x / vec3.x, y / vec3.y, z / vec3.z );
}

bool Vec3::operator==( const Vec3& compare ) const
{
    return ( x == compare.x ) && ( y == compare.y ) && ( z == compare.z );
}

bool Vec3::operator!=( const Vec3& compare ) const
{
    return !( *this == compare );
}

float Dot( const Vec3& vecA, const Vec3& vecB )
{
    return ( vecA.x * vecB.x ) + ( vecA.y * vecB.y ) + ( vecA.z * vecB.z );
}

Vec3 Cross( const Vec3& vecA, const Vec3& vecB )
{
    // remember Xyzzy
    Vec3 result;
    result.x = vecA.y * vecB.z - vecA.z * vecB.y;
    result.y = vecA.z * vecB.x - vecA.x * vecB.z;
    result.z = vecA.x * vecB.y - vecA.y * vecB.x;
    return result;
}

Vec3 SphericalToCartesian( float radius, float elevation, float azimuth )
{
    Vec3 cartesian;
    float RSinE = radius * SinDeg( elevation );
    float RCosE = radius * CosDeg( elevation );

    cartesian.x = RCosE * CosDeg( azimuth );
    cartesian.z = RCosE * SinDeg( azimuth );
    cartesian.y = RSinE;
    return cartesian;
}

Vec3 SphericalToCartesian( Vec3 spherical )
{
    return SphericalToCartesian( spherical.x, spherical.y, spherical.z );
}

Vec3 CartesianToSpherical( Vec3 position )
{
    Vec3 spherical;
    spherical.radius = position.GetLength();
    spherical.elevation = RadToDeg( asinf( position.y / spherical.radius ) );
    spherical.azimuth = Atan2Deg( position.z, position.x );
    return spherical;
}

Vec3 Slerp( const Vec3& vecA, const Vec3& vecB, float t )
{
    float aLen = vecA.GetLength();
    float bLen = vecB.GetLength();

    float len = Lerp( aLen, bLen, t );
    Vec3 unit = SlerpUnit( vecA / aLen, vecB / bLen, t );
    return len * unit;
}

Vec3 SlerpUnit( const Vec3& vecA, const Vec3& vecB, float t )
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

Vec3 Min( const Vec3& a, const Vec3& b )
{
    return Vec3( Minf( a.x, b.x ), Minf( a.y, b.y ), Minf( a.z, b.z ) );
}

Vec3 Max( const Vec3& a, const Vec3& b )
{
    return Vec3( Maxf( a.x, b.x ), Maxf( a.y, b.y ), Maxf( a.z, b.z ) );
}
