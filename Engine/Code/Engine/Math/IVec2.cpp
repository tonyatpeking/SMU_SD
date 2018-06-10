#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/IVec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"

// static values
const IVec2 IVec2::ONES       = IVec2( 1, 1 );
const IVec2 IVec2::ZEROS      = IVec2( 0, 0 );
const IVec2 IVec2::ONE_ZERO   = IVec2( 1, 0 );
const IVec2 IVec2::ZERO_ONE   = IVec2( 0, 1 );

const IVec2 IVec2::NORTH      = IVec2( 0, 1 );
const IVec2 IVec2::SOUTH      = IVec2( 0, -1 );
const IVec2 IVec2::EAST       = IVec2( 1, 0 );
const IVec2 IVec2::WEST       = IVec2( -1, 0 );
const IVec2 IVec2::NORTH_EAST = IVec2( 1, 1 );
const IVec2 IVec2::NORTH_WEST = IVec2( -1, 1 );
const IVec2 IVec2::SOUTH_EAST = IVec2( 1, -1 );
const IVec2 IVec2::SOUTH_WEST = IVec2( -1, -1 );

const IVec2 IVec2::EIGHT_DIRECTIONS[8] ={ NORTH, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST };
const IVec2 IVec2::FOUR_DIRECTIONS[4] ={ NORTH, EAST, SOUTH, WEST };

IVec2::IVec2( const IVec2 & copyFrom )
    : x( copyFrom.x )
    , y( copyFrom.y )
{
}

IVec2::IVec2( const Vec2& vec2 )
    : x( (int) vec2.x )
    , y( (int) vec2.y )
{
}

IVec2::IVec2( int initialX, int initialY )
    : x( initialX )
    , y( initialY )
{
}

IVec2::IVec2( const IVec3& ivec3 )
    : x( ivec3.x )
    , y( ivec3.y )
{

}

IVec2 IVec2::Floor( const Vec2& vec2 )
{
    return IVec2( FloorToInt( vec2.x ), FloorToInt( vec2.y ) );
}

const IVec2 IVec2::operator+( const IVec2 & vecToAdd ) const
{
    return IVec2( x + vecToAdd.x, y + vecToAdd.y );
}

const IVec2 IVec2::operator-( const IVec2 & vecToSubtract ) const
{
    return IVec2( x - vecToSubtract.x, y - vecToSubtract.y );
}

const IVec2 IVec2::operator*( int uniformScale ) const
{
    return IVec2( x * uniformScale, y * uniformScale );
}

const IVec2 IVec2::operator/( int inverseScale ) const
{
    return IVec2( x / inverseScale, y / inverseScale );
}

const IVec2 IVec2::operator-() const
{
    return IVec2( -x, -y );
}

void IVec2::operator+=( const IVec2 & vecToAdd )
{
    x += vecToAdd.x;
    y += vecToAdd.y;
}

void IVec2::operator-=( const IVec2 & vecToSubtract )
{
    x -= vecToSubtract.x;
    y -= vecToSubtract.y;
}

void IVec2::operator*=( int uniformScale )
{
    x *= uniformScale;
    y *= uniformScale;
}

void IVec2::operator/=( int uniformDivisor )
{
    x /= uniformDivisor;
    y /= uniformDivisor;
}

void IVec2::operator=( const IVec2 & copyFrom )
{
    x = copyFrom.x;
    y = copyFrom.y;
}

bool IVec2::operator==( const IVec2 & compare ) const
{
    return ( x == compare.x ) && ( y == compare.y );
}

bool IVec2::operator!=( const IVec2 & compare ) const
{
    return !( ( *this ) == compare );
}

ParseStatus IVec2::SetFromText( const String& text, const String& delimiter )
{

    std::vector<int> out_ints;
    ParseStatus status = StringUtils::ParseToInts( text, out_ints, delimiter );

    //ASSERT_RECOVERABLE( out_ints.size() == 2, "Wrong number of elements passed to IVec2" );

    if( out_ints.size() != 2 )
        return PARSE_WRONG_NUMBER_OF_TOKENS;

    x = out_ints[0];
    y = out_ints[1];

    return status;
}

Axis IVec2::AxisIfIsDirection() const
{
    if( x == 0 && y != 0 )
        return Axis::Y;
    if( y == 0 && x != 0 )
        return Axis::X;
    return Axis::INVALID_AXIS;
}

IVec2 IVec2::IndexToCoords( int index, const IVec2& dimensions )
{
    return IndexToCoords( index, dimensions.x );
}

IVec2 IVec2::IndexToCoords( int index, int dimensionX )
{
    return IVec2( index % dimensionX, index / dimensionX ); //yes we want int division
}

int IVec2::CoordsToIndex( const IVec2& coords, const IVec2& dimensions )
{
    return CoordsToIndex( coords, dimensions.x );
}

int IVec2::CoordsToIndex( const IVec2 & coords, int dimensionX )
{
    return dimensionX * coords.y + coords.x;
}


int IVec2::CoordsToIndex( int coordX, int coordY, int dimensionX )
{
    return CoordsToIndex( IVec2( coordX, coordY ), dimensionX );
}

int IVec2::GetMaxIndex( const IVec2& dimensions )
{
    return dimensions.x * dimensions.y;
}



int IVec2::GetManhattanDist( const IVec2& a, const IVec2& b )
{
    return abs( a.x - b.x ) + abs( a.y - b.y );
}

std::vector<IVec2> IVec2::GetCoordsWithinManhattanDistRange( const IVec2& center, int minRange, int maxRange, const IVec2& minBounds, const IVec2& maxBounds )
{
    std::vector<IVec2> coordsInRange;
    for( int x = minBounds.x; x <= maxBounds.x; ++x )
    {
        for( int y = minBounds.y; y <= maxBounds.y; ++y )
        {
            IVec2 tile( x, y );
            int manDistance = IVec2::GetManhattanDist( center, tile );
            if( IsInRange( manDistance, minRange, maxRange ) )
                coordsInRange.push_back( tile );
        }
    }
    return coordsInRange;
}

const IVec2 Lerp( const IVec2& start, const IVec2& end, float t )
{
    int newX = Lerp( start.x, end.x, t );
    int newY = Lerp( start.y, end.y, t );
    return IVec2( newX, newY );
}

