#include <math.h>

#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"

Disc2::Disc2( const Disc2& copyFrom )
    : center( copyFrom.center )
    , radius( copyFrom.radius )
{
}

Disc2::Disc2( float initialX, float initialY, float initialRadius )
    : center( initialX, initialY )
    , radius( initialRadius )
{
}

Disc2::Disc2( const Vec2& initialCenter, float initialRadius )
    : center( initialCenter )
    , radius( initialRadius )
{
}

void Disc2::StretchToIncludePoint( float x, float y )
{
    StretchToIncludePoint( Vec2( x, y ) );
}

void Disc2::StretchToIncludePoint( const Vec2& point )
{
    float dist = Vec2::GetDistance( center, point );
    if( dist > radius )
    {
        radius = dist;
    }
}

void Disc2::AddPaddingToRadius( float paddingRadius )
{
    radius += paddingRadius;
}

void Disc2::Translate( const Vec2& translation )
{
    center += translation;
}

void Disc2::Translate( float translationX, float translationY )
{
    Translate( Vec2( translationX, translationY ) );
}

ParseStatus Disc2::SetFromText( const String& text )
{
    std::vector<float> out_floats;
    ParseStatus status = StringUtils::ParseToFloats( text, out_floats );

    //ASSERT_RECOVERABLE( out_floats.size() == 3, "Wrong number of elements passed to Disc2" );

    if( out_floats.size() != 3 )
        return PARSE_WRONG_NUMBER_OF_TOKENS;

    center.x = out_floats[0];
    center.y = out_floats[1];
    radius = out_floats[2];

    return status;
}

bool Disc2::ContainsPoint( float x, float y ) const
{
    return ContainsPoint( Vec2( x, y ) );
}

bool Disc2::ContainsPoint( const Vec2& point ) const
{
    float dist = Vec2::GetDistance( center, point );
    return dist < radius;
}

bool Disc2::LineIntersect( const Vec2& pointA, const Vec2& pointB, Vec2& out_A, Vec2& out_B, float& out_distance )
{
    Vec2 nearestPoint = Vec2::NearestPointOnLine( center, pointA, pointB );
    out_distance = ( nearestPoint - center ).GetLength();
    if( out_distance >= radius )
        return false;
    else
    {
        // b*b = c*c - a*a
        float oppositeLength = sqrtf( ( radius * radius ) - ( out_distance * out_distance ) );
        Vec2 oppositeVector = ( pointB - pointA ).GetNormalized() * oppositeLength;
        out_A = nearestPoint - oppositeVector;
        out_B = nearestPoint + oppositeVector;
        return true;
    }
}

void Disc2::operator+=( const Vec2& translation )
{
    Translate( translation );
}

void Disc2::operator-=( const Vec2& antiTranslation )
{
    Translate( -antiTranslation );
}

Disc2 Disc2::operator+( const Vec2& translation ) const
{
    Disc2 movedCopyDisc2 = Disc2( *this );
    movedCopyDisc2.Translate( translation );
    return movedCopyDisc2;
}

Disc2 Disc2::operator-( const Vec2& antiTranslation ) const
{
    Disc2 movedCopyDisc2 = Disc2( *this );
    movedCopyDisc2.Translate( -antiTranslation );
    return movedCopyDisc2;
}

bool Disc2::IsOverlap( const Disc2& a, const Disc2& b )
{
    return IsOverlap( a.center, a.radius, b.center, b.radius );
}

bool Disc2::IsOverlap( const Vec2& aCenter, float aRadius, const Vec2& bCenter, float bRadius )
{
    float distSquared = Vec2::GetDistanceSquared( aCenter, bCenter );
    float radiusSumSquared = ( aRadius + bRadius ) * ( aRadius + bRadius );
    return distSquared < radiusSumSquared;
}

bool Disc2::IsOverlap( const Vec2& aCenter, float aRadius, const Vec2& bCenter, float bRadius, float& out_overlapLenghth )
{
    float distSquared = Vec2::GetDistanceSquared( aCenter, bCenter );
    float radiusSumSquared = ( aRadius + bRadius ) * ( aRadius + bRadius );
    if( distSquared < radiusSumSquared )
    {
        out_overlapLenghth = aRadius + bRadius - sqrtf( distSquared );
    }
    else
    {
        out_overlapLenghth = 0.f;
    }

    return distSquared < radiusSumSquared;
}

bool Disc2::IsOverlap( const Vec2& aCenter, float aRadius, const Vec2& bCenter, float bRadius, Vec2& out_overlapVectorAB )
{
    float distSquared = Vec2::GetDistanceSquared( aCenter, bCenter );
    float radiusSumSquared = ( aRadius + bRadius ) * ( aRadius + bRadius );
    if( distSquared < radiusSumSquared )
    {
        Vec2 vectorAB = ( bCenter - aCenter );
        Vec2 normalizedDir = vectorAB.GetNormalized();
        Vec2 radiusSumVector = normalizedDir * ( aRadius + bRadius );
        out_overlapVectorAB = radiusSumVector - vectorAB;
    }
    else
    {
        out_overlapVectorAB = Vec2::ZEROS;
    }

    return distSquared < radiusSumSquared;
}

const Disc2 Lerp( const Disc2& start, const Disc2& end, float t )
{
    Vec2 newCenter = Lerp( start.center, end.center, t );
    float newRadius = Lerp( start.radius, end.radius, t );
    return Disc2( newCenter, newRadius );
}
