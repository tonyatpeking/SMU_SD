#pragma once

#include <string>

#include "Engine/Math/Axis.hpp"
#include "Engine/String/ParseStatus.hpp"

class IVec3;
class Vec2;

class IVec2
{
public:

    static const IVec2 ONES;
    static const IVec2 ZEROS;
    static const IVec2 ONE_ZERO;
    static const IVec2 ZERO_ONE;

    static const IVec2 NORTH;
    static const IVec2 SOUTH;
    static const IVec2 EAST;
    static const IVec2 WEST;

    static const IVec2 NORTH_EAST;
    static const IVec2 NORTH_WEST;
    static const IVec2 SOUTH_EAST;
    static const IVec2 SOUTH_WEST;

    // These arrays list out the above directions in counter-clockwise starting from NORTH
    static const IVec2 EIGHT_DIRECTIONS[8];
    static const IVec2 FOUR_DIRECTIONS[4];


    IVec2() {};
    ~IVec2() {};
    IVec2( const IVec2& copyFrom );
    explicit IVec2( const Vec2& vec2 );
    explicit IVec2( const IVec3& ivec3 );

    explicit IVec2( int initialX, int initialY );

    static IVec2 Floor( const Vec2& vec2 );

    //operator Vec2() const { return Vec2( (float) x, (float) y ); }; // Cast (Vec2)intVec2

    const IVec2 operator+( const IVec2& vecToAdd ) const;		// vec2 + vec2
    const IVec2 operator-( const IVec2& vecToSubtract ) const;	// vec2 - vec2
    const IVec2 operator*( int uniformScale ) const;			// vec2 * int
    const IVec2 operator/( int inverseScale ) const;			// vec2 / int
    const IVec2 operator-() const;								// -vec2
    void operator+=( const IVec2& vecToAdd );						// vec2 += vec2
    void operator-=( const IVec2& vecToSubtract );				// vec2 -= vec2
    void operator*=( int uniformScale );					// vec2 *= int
    void operator/=( int uniformDivisor );					// vec2 /= int
    void operator=( const IVec2& copyFrom );						// vec2 = vec2
    bool operator==( const IVec2& compare ) const;				// vec2 == vec2
    bool operator!=( const IVec2& compare ) const;				// vec2 != vec2

    ParseStatus SetFromText( const String& text, const String& delimiter = "," );

    Axis AxisIfIsDirection() const;

    // Indexing utilities
    static IVec2 IndexToCoords( int index, const IVec2& dimensions );
    static IVec2 IndexToCoords( int index, int dimensionX );
    static int CoordsToIndex( const IVec2& coords, const IVec2& dimensions );
    static int CoordsToIndex( const IVec2& coords, int dimensionX);
    static int CoordsToIndex( int coordX, int coordY, int dimensionX );
    static int GetMaxIndex( const IVec2& dimensions );
    static int GetManhattanDist( const IVec2& a, const IVec2& b);
    //All range and bounds inclusive
    static std::vector<IVec2> GetCoordsWithinManhattanDistRange( const IVec2& center,
                                                                      int minRange, int maxRange,
                                                                      const IVec2& minBounds,
                                                                      const IVec2& maxBounds);

public:
    int x = 0;
    int y = 0;



};

const IVec2 Lerp( const IVec2& start, const IVec2& end, float t );
