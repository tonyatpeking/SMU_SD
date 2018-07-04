#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec4.hpp"

class Vec2;
class Vec3;
class Plane;

class Mat4
{
public:
    static const Mat4 IDENTITY;

    Mat4() {}; // default-construct to Identity matrix
    explicit Mat4( const float* sixteenValuesBasisMajor ); // float[16] array in order Ix, Iy...
    explicit Mat4( const Vec2& iBasis, const Vec2& jBasis, const Vec2& translation = Vec2( 0.f, 0.f ) );
    explicit Mat4( const Vec3& i, const Vec3& j, const Vec3& k, const Vec3& t );

    // Operators
    void operator=( const Mat4& copyFrom );
    const Mat4 operator*( const Mat4& rhs ) const;
    const Vec3 operator*( const Vec3& rhs ) const;
    const Vec4 operator*( const Vec4& rhs ) const;
    bool operator==( const Mat4& rhs ) const;
    bool operator!=( const Mat4& rhs ) const;

    Vec3 TransformPosition( const Vec3& position ) const;
    Vec3 TransformDisplacement( const Vec3& displacement ) const;
    Plane TransformPlane( const Plane& plane ) const;

    // Accessors
    Vec2 TransformPosition2D( const Vec2& position2D ); // Written assuming z=0, w=1
    Vec2 TransformDisplacement2D( const Vec2& displacement2D ); // Written assuming z=0, w=0
    Vec4 GetRowX() const;
    Vec4 GetRowY() const;
    Vec4 GetRowZ() const;
    Vec4 GetRowW() const;
    void DecomposeToSRT( Vec3& out_scale, Vec3& out_euler, Vec3& out_translation ) const;
    Mat4 GetRotationalPart() const;
    // Rotation part must be pure rotation (no scale), translation is ignored
    Vec3 DecomposeEuler() const;
    Vec3 DecomposeScale() const;
    bool IsAnyInf() const;
    bool IsAnyNaN() const;

    // Mutators
    void SetIdentity();
    void SetValues( const float* sixteenValuesBasisMajor ); // float[16] array in order Ix, Iy...
    void SetRowX( Vec4 rowX );
    void SetRowY( Vec4 rowY );
    void SetRowZ( Vec4 rowZ );
    void SetRowW( Vec4 rowW );
    bool Invert();
    Mat4 Inverse() const;
    void Transpose();
    Mat4 Transposed();
    void InvertTranslation();
    Mat4 InverseTranslation();
    void InvertRotation();
    Mat4 InverseRotation();
    void SnapToZero();

    // 2d
    void RotateDegrees2D( float rotationDegreesAboutZ ); //
    void Translate2D( const Vec2& translation );
    void ScaleUniform2D( float scaleXY );
    void Scale2D( float scaleX, float scaleY );

    // 3d
    void TranslateLocal( const Vec3& translation );
    void Scale( const Vec3& scale );


    // Producers
    static Mat4 MakeFromSRT( const Vec3& scale,
                             const Vec3& euler, const Vec3& translation );
    static Mat4 MakeTranslation( const Vec3& translation );
    static Mat4 MakeTranslation( float x, float y, float z );
    // roll first, then pitch, then yaw
    static Mat4 MakeRotationEuler( float yawDeg, float pitchDeg, float rollDeg );
    static Mat4 MakeRotationEuler( Vec3 euler );
    static Mat4 MakeRotationDegrees2D( float rotationDegreesAboutZ );
    static Mat4 MakeTranslation2D( const Vec2& translation );
    static Mat4 MakeScaleUniform2D( float scaleXY );
    static Mat4 MakeScale2D( float scaleX, float scaleY );
    static Mat4 MakeOrtho( const Vec2& bottomLeft, const Vec2& topRight,
                           float near, float far );
    static Mat4 MakeOrtho( float width, float height, float near, float far );
    static Mat4 MakeProjection( float fovVertDeg, float aspect,
                                float near, float far );
    static Mat4 Transpose( const Mat4& mat );

    static Mat4 LerpTransform( const Mat4& matA, const Mat4& matB, float t );
    // Matrix elements
/*
    Ix Jx Kx Tx
    Iy Jy Ky Ty
    Iz Jz Kz Tz
    Iw Jw Kw Tw
*/
#pragma warning(disable : 4201)
    union
    {
        float el[16] =
        {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
        };

        struct
        {
            Vec4 I;
            Vec4 J;
            Vec4 K;
            Vec4 T;
        };

        struct
        {
            float Ix;
            float Iy;
            float Iz;
            float Iw;

            float Jx;
            float Jy;
            float Jz;
            float Jw;

            float Kx;
            float Ky;
            float Kz;
            float Kw;

            float Tx;
            float Ty;
            float Tz;
            float Tw;
        };
    };
#pragma warning(default : 4201)

};