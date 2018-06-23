#pragma once
#include <string>

#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IVec3.hpp"
#include "Engine/Core/ParseStatus.hpp"
#include "Engine/Core/Types.hpp"

class MeshBuilder;
class Mat4;

class AABB3
{

public:

    static const AABB3 ZEROS_ONES;
    static const AABB3 NEG_ONES_ONES;

    // Constructors / destructors:
    ~AABB3() {}
    AABB3() {}
    AABB3( const AABB3& copy );
    explicit AABB3( float minX, float minY, float minZ, float maxX, float maxY,
                    float maxZ );

    explicit AABB3( const Vec3& mins, const Vec3& maxs );
    explicit AABB3( const Vec3& center, float width, float height, float depth );

    static AABB3 FromMeshBuilder( const MeshBuilder& meshBuilder, const Mat4& transform );

    // Mutators:
    void StretchToIncludePoint( const Vec3& point ); // note: stretch, not move
    void AddPaddingToSides( float xPad, float yPad, float zPad );
    void Translate( const Vec3& translation ); // move the box; similar to +=
    void Translate( float translationX, float translationY, float translationZ );
    ParseStatus SetFromText( const String& text );
    void ScaleFromCenter( float scale );
    void ScaleFromCenter( const Vec3& scale );
    void ScaleFromCenter( float scaleX, float scaleY, float scaleZ );
    void ScaleWidthFromCenter( float scaleX );
    void ScaleHeightFromCenter( float scaleY );
    void ScaleDepthFromCenter( float scaleZ );
    // localReference treats the AABB3 as a coord system,
    // with mins = localBounds.mins and maxs = localBounds.maxs in local
    void ScaleFromLocalPointInRect( const Vec3& scale, const Vec3& localReference );
    void SetWidth( float width );
    void SetHeight( float height );
    void SetDepth( float height );
    void SetCenter( const Vec3& center );

    // Accessors / queries:
    bool IsValid() const;
    bool IsPointInside( float x, float y, float z ) const;
    bool IsPointInside( const Vec3& point ) const;
    Vec3 GetDimensions() const;
    Vec3 GetCenter() const;
    // local point treats the AABB3 as a coord system,
    // with mins = (0,0) and maxs = (1,1) in local
    Vec3 RangeMap01ToBounds( const Vec3& localPoint ) const;
    Vec3 RangeMap01ToBounds( float x, float y, float z ) const;
    AABB3 RangeMap01ToBounds( const AABB3& localBounds );

    // local point treats the AABB3 as a coord system,
    // with mins = (-1,-1) and maxs = (1,1) in local
    Vec3 RangeMapNDCToBounds( const Vec3& localPoint ) const;
    AABB3 RangeMapNDCToBounds( const AABB3& localBounds );

    float GetHeight() const;
    float GetWidth() const;
    float GetDepth() const;

    // Operators:
    void operator+=( const Vec3& translation ); // move (translate) the box
    void operator-=( const Vec3& antiTranslation );
    AABB3 operator+( const Vec3& translation ) const; // create a (temp) moved box
    AABB3 operator-( const Vec3& antiTranslation ) const;
    const AABB3 operator*( float uniformScale ) const;

    // static functions
    static bool IsOverlap( const AABB3& a, const AABB3& b );
    static AABB3 MakeBoundsFromDimensions( const Vec3& dimensions );
    static AABB3 MakeBoundsFromDimensions( const IVec3& dimensions );

public:
    Vec3 mins = Vec3::POS_INFINITY;
    Vec3 maxs = Vec3::NEG_INFINITY;


};
