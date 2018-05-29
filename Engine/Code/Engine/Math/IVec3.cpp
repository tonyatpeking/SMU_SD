#include "Engine/Math/IVec3.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"

// static values
const IVec3 IVec3::ONES       = IVec3( 1, 1, 1 );
const IVec3 IVec3::ZEROS      = IVec3( 0, 0, 0 );

const IVec3 IVec3::UP              = IVec3( 0, 1, 0 );
const IVec3 IVec3::DOWN            = IVec3( 0,-1, 0 );
const IVec3 IVec3::LEFT            = IVec3(-1, 0, 0 );
const IVec3 IVec3::RIGHT           = IVec3( 1, 0, 0 );
const IVec3 IVec3::FORWARD         = IVec3( 0, 0, 1 );
const IVec3 IVec3::BACKWARD        = IVec3( 0, 0,-1 );

IVec3::IVec3( const IVec3 & copyFrom )
    : x( copyFrom.x )
    , y( copyFrom.y )
    , z( copyFrom.z )
{
}

IVec3::IVec3( const Vec3& vec3 )
    : x( (int) vec3.x )
    , y( (int) vec3.y )
    , z( (int) vec3.z )
{
}

IVec3::IVec3( int initialX, int initialY, int initialZ )
    : x( initialX )
    , y( initialY )
    , z( initialZ )
{
}

IVec3::IVec3( const IVec2& ivec2, int initialZ /*= 0 */ )
    : x( ivec2.x )
    , y( ivec2.y )
    , z( initialZ )
{

}

const IVec3 IVec3::operator+( const IVec3 & vecToAdd ) const
{
    return IVec3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z );
}

const IVec3 IVec3::operator-( const IVec3 & vecToSubtract ) const
{
    return IVec3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z );
}

const IVec3 IVec3::operator*( int uniformScale ) const
{
    return IVec3( x * uniformScale, y * uniformScale,  z * uniformScale );
}

const IVec3 IVec3::operator/( int inverseScale ) const
{
    return IVec3( x / inverseScale, y / inverseScale, z / inverseScale );
}

const IVec3 IVec3::operator-() const
{
    return IVec3( -x, -y, -z );
}

void IVec3::operator+=( const IVec3 & vecToAdd )
{
    x += vecToAdd.x;
    y += vecToAdd.y;
    z += vecToAdd.z;
}

void IVec3::operator-=( const IVec3 & vecToSubtract )
{
    x -= vecToSubtract.x;
    y -= vecToSubtract.y;
    z -= vecToSubtract.z;
}

void IVec3::operator*=( int uniformScale )
{
    x *= uniformScale;
    y *= uniformScale;
    z *= uniformScale;
}

void IVec3::operator/=( int uniformDivisor )
{
    x /= uniformDivisor;
    y /= uniformDivisor;
    z /= uniformDivisor;
}

void IVec3::operator=( const IVec3 & copyFrom )
{
    x = copyFrom.x;
    y = copyFrom.y;
    z = copyFrom.z;
}

bool IVec3::operator==( const IVec3 & compare ) const
{
    return ( x == compare.x ) && ( y == compare.y ) && ( z == compare.z );
}

bool IVec3::operator!=( const IVec3 & compare ) const
{
    return !( ( *this ) == compare );
}

ParseStatus IVec3::SetFromText( const String& text )
{
    String delimiter = ",";
    if( StringUtils::ContainsSubstring( text, "~" ) )
    {
        ERROR_RECOVERABLE( "Found '~' in IVec3, use ',' instead" );
        delimiter = "~";
    }

    std::vector<int> out_ints;
    ParseStatus status = StringUtils::ParseToInts( text, out_ints, delimiter );

    //ASSERT_RECOVERABLE( out_ints.size() == 3, "Wrong number of elements passed to IVec3" );

    if( out_ints.size() != 3 )
        return PARSE_WRONG_NUMBER_OF_TOKENS;

    x = out_ints[0];
    y = out_ints[1];
    y = out_ints[2];

    return status;
}

Axis IVec3::AxisIfIsDirection() const
{

    if( x != 0 && y == 0 && z == 0 )
        return Axis::X;
    if( x == 0 && y != 0 && z == 0 )
        return Axis::Y;
    if( x == 0 && y == 0 && z != 0 )
        return Axis::Z;
    return Axis::INVALID_AXIS;
}

IVec3 IVec3::IndexToCoords( int index, const IVec3& dimensions )
{
    int pageSize = dimensions.x * dimensions.y;
    int page = index / pageSize; // yes we want int/int
    int pageRemainder = index % pageSize;
    int row = pageRemainder / dimensions.x;
    int col = pageRemainder % dimensions.x;
    return IVec3( col, row, page ); //yes we want int division
}

int IVec3::CoordsToIndex( const IVec3& coords, const IVec3& dimensions )
{
    int pageSize = dimensions.x * dimensions.y;
    return pageSize * coords.z + dimensions.x * coords.y + coords.x;
}

int IVec3::GetMaxIndex( const IVec3& dimensions )
{
    return dimensions.x * dimensions.y * dimensions.z;
}

const IVec3 Lerp( const IVec3& start, const IVec3& end, float t )
{
    int newX = Lerp( start.x, end.x, t );
    int newY = Lerp( start.y, end.y, t );
    int newZ = Lerp( start.z, end.z, t );
    return IVec3( newX, newY, newZ );
}
