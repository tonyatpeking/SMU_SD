#pragma once

#include <string>

#include "Vec2.hpp"
#include "Engine/String/ParseStatus.hpp"

class Disc2
{
public:
    // Constructors / destructors:
    ~Disc2() {}
    Disc2() {}
    Disc2( const Disc2& copyFrom );
    explicit Disc2( float initialX, float initialY, float initialRadius );
    explicit Disc2( const Vec2& initialCenter, float initialRadius );
    // Mutators:
    void StretchToIncludePoint( float x, float y ); // expand radius if (x,y) is outside
    void StretchToIncludePoint( const Vec2& point ); // expand radius if point outside
    void AddPaddingToRadius( float paddingRadius );
    void Translate( const Vec2& translation ); // move the center
    void Translate( float translationX, float translationY ); // move the center
    ParseStatus SetFromText( const string& text );

    // Accessors / queries:
    bool ContainsPoint( float x, float y ) const; // is (x,y) within disc's interior?
    bool ContainsPoint( const Vec2& point ) const; // is "point" within disc's interior?
    bool LineIntersect( const Vec2& pointA, const Vec2& pointB, Vec2& out_A, Vec2& out_B, float& out_distance );

    // Operators:
    void operator+=( const Vec2& translation ); // move
    void operator-=( const Vec2& antiTranslation );
    Disc2 operator+( const Vec2& translation ) const; // create a moved copy
    Disc2 operator-( const Vec2& antiTranslation ) const;

    //Static functions
    static bool IsOverlap( const Disc2& a, const Disc2& b );
    static bool IsOverlap( const Vec2& aCenter, float aRadius,
                         const Vec2& bCenter, float bRadius );
    static bool IsOverlap( const Vec2& aCenter, float aRadius,
                         const Vec2& bCenter, float bRadius, float& out_overlapLenghth );
    static bool IsOverlap( const Vec2& aCenter, float aRadius,
                         const Vec2& bCenter, float bRadius, Vec2& out_overlapVectorAB ); //overlapVector points at b

public:

    Vec2 center = Vec2::ZEROS; // like Vec2, this breaks the "no public members" and "m_" naming rules
    float	radius = 0.f;
};


//stand alone functions

const Disc2 Lerp( const Disc2& start, const Disc2& end, float t );