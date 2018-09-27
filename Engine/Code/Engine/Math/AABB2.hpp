#pragma once
#include <string>

#include "Engine/Math/Vec2.hpp"
#include "Engine/String/ParseStatus.hpp"


class AABB2
{

public:

    static const AABB2 ZEROS_ONES;
    static const AABB2 NEG_ONES_ONES;

    // Constructors / destructors:
    ~AABB2() {}
    AABB2() {}
    AABB2( const AABB2& copy );
    explicit AABB2( float minX, float minY, float maxX, float maxY );
    explicit AABB2( const Vec2& mins, const Vec2& maxs );
    explicit AABB2( const Vec2& center, float width, float height );
    static AABB2 FromDimensions( const Vec2& center, const Vec2& dimentions );

    // Mutators:
    void StretchToIncludePoint( float x, float y ); // note: stretch, not move
    void StretchToIncludePoint( const Vec2& point ); // note: stretch, not move
    void AddPaddingToSides( float xPaddingRadius, float yPaddingRadius );
    void Translate( const Vec2& translation ); // move the box; similar to +=
    void Translate( float translationX, float translationY );
    void FlipY();
    ParseStatus SetFromText( const string& text, const string& delimiter = "," );
    void ScaleFromCenter( float scale );
    void ScaleFromCenter( const Vec2& scale );
    void ScaleFromCenter( float scaleX, float scaleY );
    void ScaleWidthFromCenter( float scaleX );
    void ScaleHeightFromCenter( float scaleY );
    void ScaleFromLocalPointInRect( const Vec2& scale, const Vec2& localReference );  // localReference treats the AABB2 as a coord system, with mins = (0,0) and maxs = (1,1) in local
    void SetWidth( float width );
    void SetHeight( float height );
    void SetCenter( const Vec2& center );

    // Accessors / queries:
    bool ContainsPoint( float x, float y ) const; // is "x,y" within box's interior?
    bool ContainsPoint( const Vec2& point ) const; // is "point" within box's interior?
    Vec2 GetDimensions() const; // return a Vec2 of ( width, height )
    Vec2 GetCenter() const; // return the center position of the box
    Vec2 RangeMap01ToBounds( const Vec2& localPoint ) const; // local point treats the AABB2 as a coord system, with mins = (0,0) and maxs = (1,1) in local
    Vec2 RangeMap01ToBounds( float x, float y ) const;
    Vec2 GetNormalizedPosition( const Vec2& pos ); // where is the point if mins is 0,0 and 1,1 is maxs
    AABB2 RangeMap01ToBounds( const AABB2& localBounds ) const;
    AABB2 RangeMap01ToBounds( const Vec2& localMins, const Vec2& localMaxs ) const;
    AABB2 RangeMap01ToBounds( float minX, float minY, float maxX, float maxY ) const;
    AABB2 WithoutTranslation() const ;
    AABB2 GetNormalizedBounds( const AABB2& bounds );// where is the bounds if mins is 0,0 and 1,1 is maxs
    Vec2 GetMinXMaxY() const;
    Vec2 GetMaxXMinY() const;
    float GetHeight() const;
    float GetWidth() const;
    float GetDiagonal() const;
    float Top() const { return maxs.y; };
    float Bottom() const { return mins.y; };
    float Left() const { return mins.x; };
    float Right() const { return maxs.x; };

    // Operators:
    void operator+=( const Vec2& translation ); // move (translate) the box
    void operator-=( const Vec2& antiTranslation );
    AABB2 operator+( const Vec2& translation ) const; // create a (temp) moved box
    AABB2 operator-( const Vec2& antiTranslation ) const;
    const AABB2 operator*( float uniformScale ) const;

    // static functions
    static bool IsOverlap( const AABB2& a, const AABB2& b );
    static AABB2 MakeBoundsFromDimensions( const Vec2& dimensions );
    static AABB2 MakeBoundsFromDimensions( const IVec2& dimensions );

public:
    Vec2 mins = Vec2::ZEROS; // like Vec2, this breaks the "no public members" and "m_" naming rules;
    Vec2 maxs = Vec2::ZEROS; // this is so low-level math primitives feel like built-in types (and are fast in Debug)


};
