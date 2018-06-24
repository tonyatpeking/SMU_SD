#pragma once

#include <string>
#include "Engine/Math/Axis.hpp"
#include "Engine/Core/ParseStatus.hpp"

class Vec3;
class Vec4;

class Vec2
{
public:

    // static values
    static const Vec2 ONES;
    static const Vec2 ZEROS;
    static const Vec2 ONE_ZERO;
    static const Vec2 ZERO_ONE;
    static const Vec2 NEG_ONES;
    static const Vec2 NEG_ONE_ONE;
    static const Vec2 POINT_FIVES;

    static const Vec2 NORTH;
    static const Vec2 SOUTH;
    static const Vec2 EAST;
    static const Vec2 WEST;
    static const Vec2 NORTH_EAST;
    static const Vec2 NORTH_WEST;
    static const Vec2 SOUTH_EAST;
    static const Vec2 SOUTH_WEST;

    // Construction/Destruction
    ~Vec2() {}											// destructor: do nothing (for speed)
    Vec2() {}											// default constructor: do nothing (for speed)
    Vec2( const Vec2& copyFrom );						// copy constructor (from another vec2)
    explicit Vec2( float initialX, float initialY );		// explicit constructor (from x, y)
    explicit Vec2( const Vec3& vec3 );
    explicit Vec2( const Vec4& vec4 );
    explicit Vec2( const IVec2& ivec2 );

    // Operators
    const Vec2 operator+( const Vec2& vecToAdd ) const;		// vec2 + vec2
    const Vec2 operator-( const Vec2& vecToSubtract ) const;	// vec2 - vec2
    const Vec2 operator*( float uniformScale ) const;			// vec2 * float
    const Vec2 operator/( float inverseScale ) const;			// vec2 / float
    const Vec2 operator-() const;								// -vec2
    const Vec2 operator*( const Vec2& vec2 ) const;           // component wise multiply
    const Vec2 operator/( const Vec2& vec2 ) const;           // component wise divide
    void operator+=( const Vec2& vecToAdd );						// vec2 += vec2
    void operator-=( const Vec2& vecToSubtract );				// vec2 -= vec2
    void operator*=( float uniformScale );					// vec2 *= float
    friend const Vec2 operator*( float uniformScale, const Vec2& vecToScale );	// float * vec2
    void operator/=( float uniformDivisor );					// vec2 /= float
    void operator=( const Vec2& copyFrom );						// vec2 = vec2
    bool operator==( const Vec2& compare ) const;				// vec2 == vec2
    bool operator!=( const Vec2& compare ) const;				// vec2 != vec2

    // Accessors
    float GetLength() const;
    float GetLengthSquared() const; // faster than GetLength() since it skips the sqrtf()
    float NormalizeAndGetLength(); // set my new length to 1.0f; keep my direction
    Vec2 GetNormalized() const; // return a new vector, which is a normalized copy of me
    Vec2 GetOrthogonal() const; // return new vector, rotated 90 degrees
    Vec2 GetClosestCardinalDir();
    float GetOrientationDegrees() const; // return 0 for east (5,0), 90 for north (0,8), etc.
    float GetValueOnAxis( Axis axis ) const;

    // Mutators
    void RotateDegrees( float degrees );
    ParseStatus SetFromText( const String& text, const String& delimiter = "," );
    void Scale( const Vec2& scale );
    void SnapToZero();

    // Static
    static Vec2 Reflect( const Vec2& vectorToReflect, const Vec2& reflectionNormal );
    static Vec2 MakeVectorOnAxis( Axis axis, float lengthOnAxis=1.0f );// construct a vector based on axis
    static Vec2 MakeFromXZ( const Vec3& vec3 );
    static Vec2 MakeDirectionAtDegrees( float degrees ); // create vector at angle

    static bool IsPointLeftOfLine( const Vec2& lineStart, const Vec2& lineEnd, const Vec2& point );
    static float GetDistance( const Vec2& a, const Vec2& b );
    static float GetDistanceSquared( const Vec2& a, const Vec2& b );

    // Geometry

    // The point on a line nearest to a point, line is defined by 2 points, in other words, projection
    static Vec2 NearestPointOnLine( const Vec2& point, const Vec2& lineA, const Vec2& lineB );

    // The distance from a point to a line, line is defined by 2 points
    static float PointToLineDist( const Vec2& point, const Vec2& lineA, const Vec2& lineB );

    //2D Vector projection / transformation functions
    // Gets the projected vector in the "projectOnto" direction, whose magnitude is the projected length of "vectorToProject" in that direction.
    static Vec2 GetProjectedVector( const Vec2& vectorToProject, const Vec2& projectOnto );  //v2 is projected onto v1

    // Returns the vector's representation/coordinates in (i,j) space (instead of its original x,y space)
    static const Vec2 GetTransformedIntoBasis( const Vec2& originalVector,
                                           const Vec2& newBasisI, const Vec2& newBasisJ );

    // Takes "vectorInBasis" in (i,j) space and returns the equivalent vector in [axis-aligned] (x,y) Cartesian space
    static const Vec2 GetTransformedOutOfBasis( const Vec2& vectorInBasis,
                                            const Vec2& oldBasisI, const Vec2& oldBasisJ );

    // Decomposes "originalVector" into two component vectors, which add up to the original:
    //   "vectorAlongI" is the vector portion in the "newBasisI" direction, and
    //   "vectorAlongJ" is the vector portion in the "newBasisJ" direction.
    static void DecomposeVectorIntoBasis( const Vec2& originalVector,
                                   const Vec2& newBasisI, const Vec2& newBasisJ,
                                   Vec2& out_vectorAlongI, Vec2& out_vectorAlongJ );

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule

#pragma warning(disable : 4201)
    union
    {
        float el[2] = { 0.f,0.f };
        struct
        {
            float x ;
            float y ;
        };
        struct
        {
            float u ;
            float v ;
        };
    };
#pragma warning(default : 4201)

};


float Dot( const Vec2& v1, const Vec2& v2 );
// not really a cross product but useful for determining if point is left or right of line
float Cross( const Vec2& v1, const Vec2& v2 );

// component wise min
Vec2 Min( const Vec2& vecA, const Vec2& vecB );
Vec2 Max( const Vec2& vecA, const Vec2& vecB );

