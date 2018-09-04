#pragma once

#include <string>

#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Axis.hpp"
#include "Engine/String/ParseStatus.hpp"

class IVec3
{
public:

    static const IVec3 ONES;
    static const IVec3 ZEROS;

    static const IVec3 UP;
    static const IVec3 DOWN;
    static const IVec3 LEFT;
    static const IVec3 RIGHT;
    static const IVec3 FORWARD;
    static const IVec3 BACKWARD;


    IVec3() {};
    ~IVec3() {};
    IVec3( const IVec3& copyFrom );
    explicit IVec3( const Vec3& vec3 );
    explicit IVec3( const IVec2& ivec2, int initialZ = 0 );

    explicit IVec3( int initialX, int initialY, int initialZ );
    operator Vec3() const { return Vec3( (float) x, (float) y, (float) z ); };

    const IVec3 operator+( const IVec3& vecToAdd ) const;
    const IVec3 operator-( const IVec3& vecToSubtract ) const;
    const IVec3 operator*( int uniformScale ) const;
    const IVec3 operator/( int inverseScale ) const;
    const IVec3 operator-() const;
    void operator+=( const IVec3& vecToAdd );
    void operator-=( const IVec3& vecToSubtract );
    void operator*=( int uniformScale );
    void operator/=( int uniformDivisor );
    void operator=( const IVec3& copyFrom );
    bool operator==( const IVec3& compare ) const;
    bool operator!=( const IVec3& compare ) const;

    ParseStatus SetFromText( const String& text );

    Axis AxisIfIsDirection() const;

    // Indexing utilities
    static IVec3 IndexToCoords( int index, const IVec3& dimensions );
    static int CoordsToIndex( const IVec3& coords, const IVec3& dimensions );
    static int GetMaxIndex( const IVec3& dimensions );

public:
    int x = 0;
    int y = 0;
    int z = 0;


};

const IVec3 Lerp( const IVec3& start, const IVec3& end, float t );