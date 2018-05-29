#include <vector>

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

const AABB2 AABB2::ZEROS_ONES = AABB2( 0, 0, 1, 1 );
const AABB2 AABB2::NEG_ONES_ONES = AABB2( -1, -1, 1, 1 );

AABB2::AABB2( const AABB2& copy )
    : mins( copy.mins )
    , maxs( copy.maxs )
{
}

AABB2::AABB2( float minX, float minY, float maxX, float maxY )
    : mins( minX, minY )
    , maxs( maxX, maxY )
{
}

AABB2::AABB2( const Vec2& mins, const Vec2& maxs )
    : mins( mins )
    , maxs( maxs )
{
}

AABB2::AABB2( const Vec2& center, float width, float height )
    : mins( center )
    , maxs( center )
{
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    mins.x -= halfWidth;
    mins.y -= halfHeight;
    maxs.x += halfWidth;
    maxs.y += halfHeight;
}


AABB2 AABB2::FromDimensions( const Vec2& center, const Vec2& dim )
{
    return AABB2( center, dim.x, dim.y );
}

//Mutators:

void AABB2::StretchToIncludePoint( float x, float y )
{
    mins.x = Minf( mins.x, x );
    mins.y = Minf( mins.y, y );
    maxs.x = Maxf( maxs.x, x );
    maxs.y = Maxf( maxs.y, y );
}

void AABB2::StretchToIncludePoint( const Vec2& point )
{
    StretchToIncludePoint( point.x, point.y );
}

void AABB2::AddPaddingToSides( float xPaddingRadius, float yPaddingRadius )
{
    mins.x -= xPaddingRadius;
    mins.y -= yPaddingRadius;
    maxs.x += xPaddingRadius;
    maxs.y += yPaddingRadius;
}

void AABB2::Translate( const Vec2& translation )
{
    mins += translation;
    maxs += translation;
}

void AABB2::Translate( float translationX, float translationY )
{
    mins.x += translationX;
    mins.y += translationY;
    maxs.x += translationX;
    maxs.y += translationY;
}

void AABB2::FlipY()
{
    float temp = mins.y;
    mins.y = maxs.y;
    maxs.y = temp;
}


ParseStatus AABB2::SetFromText( const String& text, const String& delimiter )
{
    std::vector<float> out_floats;
    ParseStatus status = StringUtils::ParseToFloats( text, out_floats, delimiter );

    //ASSERT_RECOVERABLE( out_floats.size() == 4, "Wrong number of elements passed to AABB2" );
    if( out_floats.size() != 4 )
        return PARSE_WRONG_NUMBER_OF_TOKENS;

    mins.x = out_floats[0];
    mins.y = out_floats[1];
    maxs.x = out_floats[2];
    maxs.y = out_floats[3];

    return status;
}


void AABB2::ScaleFromCenter( float scale )
{
    Vec2 center = GetCenter();
    Vec2 centerToMins = mins - center;
    centerToMins *= scale;
    mins = center + centerToMins;
    maxs = center - centerToMins;
}

void AABB2::ScaleFromCenter( const Vec2& scale )
{
    Vec2 center = GetCenter();
    Vec2 centerToMins = mins - center;
    centerToMins =  centerToMins * scale;
    mins = center + centerToMins;
    maxs = center - centerToMins;
}

void AABB2::ScaleFromCenter( float scaleX, float scaleY )
{
    ScaleFromCenter( Vec2( scaleX, scaleY ) );
}

void AABB2::ScaleWidthFromCenter( float scaleX )
{
    float centerX = GetCenter().x;
    float centerToMinsX = mins.x - centerX;
    centerToMinsX *= scaleX;
    mins.x = centerX + centerToMinsX;
    maxs.x = centerX - centerToMinsX;
}

void AABB2::ScaleHeightFromCenter( float scaleY )
{
    float centerY = GetCenter().y;
    float centerToMinsY = mins.y - centerY;
    centerToMinsY *= scaleY;
    mins.y = centerY + centerToMinsY;
    maxs.y = centerY - centerToMinsY;
}

void AABB2::ScaleFromLocalPointInRect( const Vec2& scale, const Vec2& localReference )
{
    Vec2 worldRef = RangeMap01ToBounds( localReference );
    Vec2 refToMins = mins - worldRef;
    refToMins = refToMins * scale;
    mins = worldRef + refToMins;

    Vec2 refToMaxs = maxs - worldRef;
    refToMaxs = refToMaxs * scale;
    maxs = worldRef + refToMaxs;
}

void AABB2::SetWidth( float width )
{
    float centerX = ( maxs.x + mins.x ) / 2.f;
    float halfWidth = width / 2.f;
    mins.x = centerX - halfWidth;
    maxs.x = centerX + halfWidth;
}

void AABB2::SetHeight( float height )
{
    float centerY = ( maxs.y + mins.y ) / 2.f;
    float halfHeight = height / 2.f;
    mins.y = centerY - halfHeight;
    maxs.y = centerY + halfHeight;
}

void AABB2::SetCenter( const Vec2& center )
{
    Vec2 oldCenter = GetCenter();
    Vec2 oldToNew = center - oldCenter;
    mins += oldToNew;
    maxs += oldToNew;
}

//Accessors / queries:

bool AABB2::IsPointInside( float x, float y ) const
{
    return ( x > mins.x ) && ( x < maxs.x ) && ( y > mins.y ) && ( y < maxs.y );
}

bool AABB2::IsPointInside( const Vec2& point ) const
{
    return IsPointInside( point.x, point.y );
}

Vec2 AABB2::GetDimensions() const
{
    return maxs - mins;
}

Vec2 AABB2::GetCenter() const
{
    return ( mins + maxs ) / 2.f;
}

Vec2 AABB2::RangeMap01ToBounds( const Vec2& localPoint ) const
{
    Vec2 iBasisInWorld = Vec2( GetWidth(), 0 );
    Vec2 jBasisInWorld = Vec2( 0, GetHeight() );

    return Vec2::GetTransformedOutOfBasis( localPoint, iBasisInWorld, jBasisInWorld ) + mins;
}

Vec2 AABB2::RangeMap01ToBounds( float x, float y ) const
{
    return RangeMap01ToBounds( Vec2( x, y ) );
}

AABB2 AABB2::RangeMap01ToBounds( const Vec2& localMins, const Vec2& localMaxs ) const
{
    Vec2 newMins = RangeMap01ToBounds( localMins );
    Vec2 newMaxs = RangeMap01ToBounds( localMaxs );
    return AABB2( newMins, newMaxs );
}

AABB2 AABB2::RangeMap01ToBounds( float minX, float minY, float maxX, float maxY ) const
{
    return RangeMap01ToBounds( Vec2( minX, minY ), Vec2( maxX, maxY ) );
}

AABB2 AABB2::RangeMap01ToBounds( const AABB2& localBounds ) const
{
    return RangeMap01ToBounds( localBounds.mins, localBounds.maxs );
}

AABB2 AABB2::WithoutTranslation() const
{
    Vec2 center = GetCenter();
    return AABB2( mins - center, maxs - center );
}

Vec2 AABB2::GetMinXMaxY() const
{
    return Vec2( mins.x, maxs.y );
}

Vec2 AABB2::GetMaxXMinY() const
{
    return Vec2( maxs.x, mins.y );
}

float AABB2::GetHeight() const
{
    return maxs.y - mins.y;
}

float AABB2::GetWidth() const
{
    return maxs.x - mins.x;
}

//Operators:

void AABB2::operator+=( const Vec2& translation )
{
    Translate( translation );
}

void AABB2::operator-=( const Vec2& antiTranslation )
{
    Translate( -antiTranslation );
}



const AABB2 AABB2::operator*( float uniformScale ) const
{
    return AABB2( mins * uniformScale, maxs * uniformScale );
}

AABB2 AABB2::operator+( const Vec2& translation ) const
{
    AABB2 tempMovedAABB2 = AABB2( *this );
    tempMovedAABB2.Translate( translation );
    return tempMovedAABB2;
}

AABB2 AABB2::operator-( const Vec2& antiTranslation ) const
{
    return *this + ( -antiTranslation );
}


//stand alone functions

bool AABB2::IsOverlap( const AABB2& a, const AABB2& b )
{
    bool overlapHorizontal = !( ( a.mins.x > b.maxs.x ) || ( b.mins.x > a.maxs.x ) );
    bool overlapVertical = !( ( a.mins.y > b.maxs.y ) || ( b.mins.y > a.maxs.y ) );
    return overlapHorizontal && overlapVertical;
}

AABB2 AABB2::MakeBoundsFromDimensions( const Vec2& dimensions )
{
    return AABB2( Vec2::ZEROS, dimensions );
}

AABB2 AABB2::MakeBoundsFromDimensions( const IVec2& dimensions )
{
    return MakeBoundsFromDimensions( (Vec2) dimensions );
}

