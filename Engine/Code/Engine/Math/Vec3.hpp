#pragma once
#include "Engine/Math/Axis.hpp"

class Vec2;
class Vec4;


class Vec3
{
public:

    // static values
    static const Vec3 UP;
    static const Vec3 DOWN;
    static const Vec3 LEFT;
    static const Vec3 RIGHT;
    static const Vec3 FORWARD;
    static const Vec3 BACKWARD;

    static const Vec3 ZEROS;
    static const Vec3 ONES;
    static const Vec3 NEG_ONES;
    static const Vec3 POS_INFINITY;
    static const Vec3 NEG_INFINITY;


    explicit Vec3( float initX = 0.f, float initY = 0.f, float initZ = 0.f );
    explicit Vec3( const Vec2& vec2, float appendZ = 0.f );
    explicit Vec3( const Vec4& vec4 );
    static Vec3 MakeFromXZ( const Vec2& vec2, float y = 0.f );
    Vec2 ToVec2();
    Vec2 ToVec2XZ();


    // Operators
    const Vec3 operator+( const Vec3& vecToAdd ) const;		// vec3 + vec3
    const Vec3 operator-( const Vec3& vecToSubtract ) const;	// vec3 - vec3
    const Vec3 operator*( float uniformScale ) const;			// vec3 * float
    const Vec3 operator/( float inverseScale ) const;			// vec3 / float
    const Vec3 operator-() const;								// -vec3
    const Vec3 operator*( const Vec3& vec3 ) const;			// component wise
    const Vec3 operator/( const Vec3& vec3 ) const;			// component wise
    void operator+=( const Vec3& vecToAdd );						// vec3 += vec3
    void operator-=( const Vec3& vecToSubtract );				// vec3 -= vec3
    void operator*=( float uniformScale );					// vec3 *= float
    friend const Vec3 operator*( float uniformScale, const Vec3& vecToScale );	// float * vec3
    void operator/=( float uniformDivisor );					// vec3 /= float
    void operator=( const Vec3& copyFrom );						// vec3 = vec3
    bool operator==( const Vec3& compare ) const;				// vec3 == vec3
    bool operator!=( const Vec3& compare ) const;				// vec3 != vec3

    // Operations
    float GetLength() const;
    float GetLengthSquared() const;
    float NormalizeAndGetLength();
    Vec3 GetNormalized() const;
    Vec3 GetClosestCardinalDir();
    float GetValueOnAxis( Axis axis ) const;
    void SnapToZero();

    ParseStatus SetFromText( const String& text, const String& delimiter = "," );

    static Vec3 Reflect( const Vec3& vectorToReflect, const Vec3& reflectionNormal );
    static Vec3 MakeVectorOnAxis( Axis axis, float lengthOnAxis=1.0f );// construct a vector based on axis
    static float GetDistance( const Vec3& a, const Vec3& b );
    static float GetDistanceSquared( const Vec3& a, const Vec3& b );
    static float GetAngleBetween( const Vec3& a, const Vec3& b );

    // Geometry

    // The point on a line nearest to a point, line is defined by 2 points, in other words, projection
    static Vec3 NearestPointOnLine( const Vec3& point, const Vec3& lineStart, const Vec3& lineEnd );

    // The distance from a point to a line, line is defined by 2 points
    static float PointToLineDist( const Vec3& point, const Vec3& lineStart, const Vec3& lineEnd );
    static float PointToLineDistSq( const Vec3& point, const Vec3& lineStart, const Vec3& lineEnd );

    //2D Vector projection / transformation functions
    // Gets the projected vector in the "projectOnto" direction, whose magnitude is the projected length of "vectorToProject" in that direction.
    static Vec3 GetProjectedVector( const Vec3& vectorToProject, const Vec3& projectOnto );
    static Vec3 GetProjectedToPlane( const Vec3& vectorToProject, const Vec3& planeNormal );  //v2 is projected onto v1

                                                                                             // Returns the vector's representation/coordinates in (i,j) space (instead of its original x,y space)
    static const Vec3 GetTransformedIntoBasis( const Vec3& originalVector,
                                               const Vec3& newBasisI,
                                               const Vec3& newBasisJ,
                                               const Vec3& newBasisK );

    // Takes "vectorInBasis" in (i,j) space and returns the equivalent vector in [axis-aligned] (x,y) Cartesian space
    static const Vec3 GetTransformedOutOfBasis( const Vec3& vectorInBasis,
                                                const Vec3& oldBasisI,
                                                const Vec3& oldBasisJ,
                                                const Vec3& oldBasisK );

    // Decomposes "originalVector" into two component vectors, which add up to the original:
    //   "vectorAlongI" is the vector portion in the "newBasisI" direction, and
    //   "vectorAlongJ" is the vector portion in the "newBasisJ" direction.
    static void DecomposeVectorIntoBasis( const Vec3& originalVector,
                                          const Vec3& newBasisI,
                                          const Vec3& newBasisJ,
                                          const Vec3& newBasisK,
                                          Vec3& out_vectorAlongI,
                                          Vec3& out_vectorAlongJ,
                                          Vec3& out_vectorAlongK );

#pragma warning(disable : 4201)
    union
    {
        float el[3] ={ 0.f,0.f,0.f };
        struct
        {
            float x;
            float y;
            float z;
        };
        struct
        {
            float r;
            float g;
            float b;
        };
        struct
        {
            float radius;
            float elevation;
            float azimuth;
        };
    };
#pragma warning(default : 4201)

};


// free functions
float Dot( const Vec3& vecA, const Vec3& vecB );
Vec3 Cross( const Vec3& vecA, const Vec3& vecB );
Vec3 SphericalToCartesian( float radius, float elevation, float azimuth );
Vec3 SphericalToCartesian( Vec3 spherical );
Vec3 CartesianToSpherical( Vec3 position );
Vec3 Slerp( const Vec3& vecA, const Vec3& vecB, float t );
Vec3 SlerpUnit( const Vec3& vecA, const Vec3& vecB, float t );
// component wise min
Vec3 Min( const Vec3& vecA, const Vec3& vecB );
Vec3 Max( const Vec3& vecA, const Vec3& vecB );

