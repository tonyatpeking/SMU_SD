#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/StringUtils.hpp"


const AABB3 AABB3::ZEROS_ONES = AABB3( 0, 0, 0, 1, 1, 1 );

const AABB3 AABB3::NEG_ONES_ONES = AABB3( -1, -1, -1, 1, 1, 1 );

AABB3::AABB3( const AABB3& copy )
    : mins( copy.mins )
    , maxs( copy.maxs )
{
}

AABB3::AABB3( float minX, float minY, float minZ, float maxX, float maxY, float maxZ )
    : mins( minX, minY, minZ )
    , maxs( maxX, maxY, maxZ )
{
}

AABB3::AABB3( const Vec3& mins, const Vec3& maxs )
    : mins( mins )
    , maxs( maxs )
{

}

AABB3::AABB3( const Vec3& center, float width, float height, float depth )
    : mins( center )
    , maxs( center )
{
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    float halfDepth = depth * 0.5f;
    mins.x -= halfWidth;
    mins.y -= halfHeight;
    mins.z -= halfDepth;
    maxs.x += halfWidth;
    maxs.y += halfHeight;
    maxs.z += halfDepth;
}

void AABB3::StretchToIncludePoint( float x, float y, float z )
{
    mins.x = Minf( mins.x, x );
    mins.y = Minf( mins.y, y );
    mins.z = Minf( mins.z, z );

    maxs.x = Maxf( maxs.x, x );
    maxs.y = Maxf( maxs.y, y );
    maxs.z = Maxf( maxs.z, z );
}

void AABB3::StretchToIncludePoint( const Vec3& point )
{
    StretchToIncludePoint( point.x, point.y, point.z );
}

void AABB3::AddPaddingToSides( float xPad, float yPad, float zPad )
{
    mins.x -= xPad;
    mins.y -= yPad;
    mins.z -= zPad;

    maxs.x += xPad;
    maxs.y += yPad;
    maxs.z += zPad;
}

void AABB3::Translate( const Vec3& translation )
{
    mins += translation;
    maxs += translation;
}

void AABB3::Translate( float translationX, float translationY, float translationZ )
{
    mins.x += translationX;
    mins.y += translationY;
    mins.z += translationZ;
    maxs.x += translationX;
    maxs.y += translationY;
    maxs.z += translationZ;
}

ParseStatus AABB3::SetFromText( const String& text )
{
    std::vector<float> out_floats;
    ParseStatus status = StringUtils::ParseToFloats( text, out_floats );

    //ASSERT_RECOVERABLE( out_floats.size() == 4, "Wrong number of elements passed to AABB2" );
    if( out_floats.size() != 6 )
        return PARSE_WRONG_NUMBER_OF_TOKENS;

    mins.x = out_floats[0];
    mins.y = out_floats[1];
    mins.z = out_floats[2];
    maxs.x = out_floats[3];
    maxs.y = out_floats[4];
    maxs.z = out_floats[5];

    return status;
}

void AABB3::ScaleFromCenter( float scale )
{
    Vec3 center = GetCenter();
    Vec3 centerToMins = mins - center;
    centerToMins *= scale;
    mins = center + centerToMins;
    maxs = center - centerToMins;
}

void AABB3::ScaleFromCenter( const Vec3& scale )
{
    Vec3 center = GetCenter();
    Vec3 centerToMins = mins - center;
    centerToMins =  centerToMins * scale;
    mins = center + centerToMins;
    maxs = center - centerToMins;
}

void AABB3::ScaleFromCenter( float scaleX, float scaleY, float scaleZ )
{
    ScaleFromCenter( Vec3( scaleX, scaleY, scaleZ ) );
}

void AABB3::ScaleWidthFromCenter( float scaleX )
{
    ScaleFromCenter( scaleX, 1, 1 );
}

void AABB3::ScaleHeightFromCenter( float scaleY )
{
    ScaleFromCenter( 1, scaleY, 1 );
}

void AABB3::ScaleDepthFromCenter( float scaleZ )
{
    ScaleFromCenter( 1, 1, scaleZ );
}

void AABB3::ScaleFromLocalPointInRect( const Vec3& scale, const Vec3& localReference )
{
    Vec3 worldRef = RangeMap01ToBounds( localReference );
    Vec3 refToMins = mins - worldRef;
    refToMins = refToMins * scale;
    mins = worldRef + refToMins;

    Vec3 refToMaxs = maxs - worldRef;
    refToMaxs = refToMaxs * scale;
    maxs = worldRef + refToMaxs;
}

void AABB3::SetWidth( float width )
{
    float centerX = ( maxs.x + mins.x ) / 2.f;
    float halfWidth = width / 2.f;
    mins.x = centerX - halfWidth;
    maxs.x = centerX + halfWidth;
}

void AABB3::SetHeight( float height )
{
    float centerY = ( maxs.y + mins.y ) / 2.f;
    float halfHeight = height / 2.f;
    mins.y = centerY - halfHeight;
    maxs.y = centerY + halfHeight;
}

void AABB3::SetDepth( float depth )
{
    float centerZ = ( maxs.z + mins.z ) / 2.f;
    float halfDepth = depth / 2.f;
    mins.z = centerZ - halfDepth;
    maxs.z = centerZ + halfDepth;
}

void AABB3::SetCenter( const Vec3& center )
{
    Vec3 oldCenter = GetCenter();
    Vec3 oldToNew = center - oldCenter;
    mins += oldToNew;
    maxs += oldToNew;
}

bool AABB3::IsPointInside( float x, float y, float z ) const
{
    return ( x > mins.x ) && ( x < maxs.x )
        && ( y > mins.y ) && ( y < maxs.y )
        && ( z > mins.z ) && ( z < maxs.z );
}

bool AABB3::IsPointInside( const Vec3& point ) const
{
    return IsPointInside( point.x, point.y, point.z );
}

Vec3 AABB3::GetDimensions() const
{
    return maxs - mins;
}

Vec3 AABB3::GetCenter() const
{
    return ( mins + maxs ) / 2.f;
}

Vec3 AABB3::RangeMap01ToBounds( const Vec3& localPoint ) const
{
    Vec3 iBasisInWorld = Vec3( GetWidth(), 0, 0 );
    Vec3 jBasisInWorld = Vec3( 0, GetHeight(), 0 );
    Vec3 kBasisInWorld = Vec3( 0, 0, GetDepth() );

    return Vec3::GetTransformedOutOfBasis(
        localPoint, iBasisInWorld, jBasisInWorld, kBasisInWorld ) + mins;
}

Vec3 AABB3::RangeMap01ToBounds( float x, float y, float z ) const
{
    return RangeMap01ToBounds( Vec3( x, y, z ) );
}

AABB3 AABB3::RangeMap01ToBounds( const AABB3& localBounds )
{
    Vec3 newMins = RangeMap01ToBounds( localBounds.mins );
    Vec3 newMaxs = RangeMap01ToBounds( localBounds.maxs );
    return AABB3( newMins, newMaxs );
}

Vec3 AABB3::RangeMapNDCToBounds( const Vec3& localPoint ) const
{
    Vec3 iBasisInWorld = Vec3( GetWidth(), 0, 0 );
    Vec3 jBasisInWorld = Vec3( 0, GetHeight(), 0 );
    Vec3 kBasisInWorld = Vec3( 0, 0, GetDepth() );

    return Vec3::GetTransformedOutOfBasis(
        localPoint, iBasisInWorld, jBasisInWorld, kBasisInWorld ) + GetCenter();
}

AABB3 AABB3::RangeMapNDCToBounds( const AABB3& localBounds )
{
    Vec3 newMins = RangeMapNDCToBounds( localBounds.mins );
    Vec3 newMaxs = RangeMapNDCToBounds( localBounds.maxs );
    return AABB3( newMins, newMaxs );
}

float AABB3::GetHeight() const
{
    return maxs.y - mins.y;
}

float AABB3::GetWidth() const
{
    return maxs.x - mins.x;
}

float AABB3::GetDepth() const
{
    return maxs.z - mins.z;
}

void AABB3::operator-=( const Vec3& antiTranslation )
{
    Translate( -antiTranslation );
}

AABB3 AABB3::operator-( const Vec3& antiTranslation ) const
{
    return *this + ( -antiTranslation );
}

const AABB3 AABB3::operator*( float uniformScale ) const
{
    return AABB3( mins * uniformScale, maxs * uniformScale );
}

bool AABB3::IsOverlap( const AABB3& a, const AABB3& b )
{
    bool overlapHorizontal = !( ( a.mins.x > b.maxs.x ) || ( b.mins.x > a.maxs.x ) );
    bool overlapVertical = !( ( a.mins.y > b.maxs.y ) || ( b.mins.y > a.maxs.y ) );
    bool overlapDepth = !( ( a.mins.z > b.maxs.z ) || ( b.mins.z > a.maxs.z ) );

    return overlapHorizontal && overlapVertical && overlapDepth;
}

AABB3 AABB3::MakeBoundsFromDimensions( const Vec3& dimensions )
{
    return AABB3( Vec3::ZEROS, dimensions );
}

AABB3 AABB3::MakeBoundsFromDimensions( const IVec3& dimensions )
{
    return MakeBoundsFromDimensions( (Vec3) dimensions );
}

void AABB3::operator+=( const Vec3& translation )
{
    Translate( translation );
}

AABB3 AABB3::operator+( const Vec3& translation ) const
{
    AABB3 tempMovedAABB3 = AABB3( *this );
    tempMovedAABB3.Translate( translation );
    return tempMovedAABB3;
}