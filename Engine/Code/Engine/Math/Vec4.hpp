#pragma once

class Vec3;
class Vec2;

class Vec4
{
public:

    static const Vec4 ZEROS;
    static const Vec4 ZEROS_W_ONE;
    static const Vec4 ONES;
    static const Vec4 NEG_ONES;

    explicit Vec4( float initX = 0.f, float initY = 0.f, float initZ = 0.f, float initW = 0.f );
    explicit Vec4( const Vec3& vec3, float appendW = 0.f );
    explicit Vec4( const Vec2& vec2, float appendZ = 0.f, float appendW = 0.f );
    // Operators
    const Vec4 operator+( const Vec4& vecToAdd ) const;		// vec3 + vec3
    const Vec4 operator-( const Vec4& vecToSubtract ) const;	// vec3 - vec3
    const Vec4 operator*( float uniformScale ) const;			// vec3 * float
    const Vec4 operator/( float inverseScale ) const;			// vec3 / float
    const Vec4 operator-() const;								// -vec3
    void operator+=( const Vec4& vecToAdd );						// vec3 += vec3
    void operator-=( const Vec4& vecToSubtract );				// vec3 -= vec3
    void operator*=( float uniformScale );					// vec3 *= float
    friend const Vec4 operator*( float uniformScale, const Vec4& vecToScale );	// float * vec3
    void operator/=( float uniformDivisor );					// vec3 /= float
    void operator=( const Vec4& copyFrom );						// vec3 = vec3
    bool operator==( const Vec4& compare ) const;				// vec3 == vec3
    bool operator!=( const Vec4& compare ) const;				// vec3 != vec3
    Vec3 XYZ() const;

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
    float w = 0.f;
};

float Dot( const Vec4& vecA, const Vec4& vecB );
