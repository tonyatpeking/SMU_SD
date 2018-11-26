#include <math.h>
#include <cmath>

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/FileIO/XmlUtils.hpp"
#include "Engine/String/StringUtils.hpp"

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IRange.hpp"
#include "Engine/Math/Range.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"


bool AlmostZero( float f, float eps )
{
    return fabsf( f ) < eps;
}

float SnapToZero( float f )
{
    if( AlmostZero( f ) )
        return 0.f;
    return f;
}

bool AlmostEqual( const float a, const float b )
{
    return fabsf( a - b ) < EPSILON;
}

bool AlmostEqual( const Vec2& a, const Vec2& b )
{
    return AlmostEqual( a.x, b.x ) && AlmostEqual( a.y, b.y );
}

bool AlmostEqual( const AABB2& a, const AABB2& b )
{
    return AlmostEqual( a.mins, b.mins ) && AlmostEqual( a.maxs, b.maxs );
}

bool AlmostEqual( const Range& a, const Range& b )
{
    return AlmostEqual( a.min, b.min ) && AlmostEqual( a.max, b.max );
}

bool AlmostEqual( const Disc2& a, const Disc2& b )
{
    return AlmostEqual( a.radius, b.radius ) && AlmostEqual( a.center, b.center );
}

float RadToDeg( const float rad )
{
    return rad * ( 180.f / (float) M_PI );
}

float DegToRad( const float deg )
{
    return deg * ( (float) M_PI / 180.f );
}

float CosDeg( const float deg )
{
    return cosf( DegToRad( deg ) );
}

float SinDeg( const float deg )
{
    return sinf( DegToRad( deg ) );
}

float TanDeg( const float deg )
{
    return tanf( DegToRad( deg ) );
}

float Atan2Deg( float y, float x )
{
    return RadToDeg( atan2f( y, x ) );
}

float ArcSinDeg( float sinTheta )
{
    sinTheta = Clampf( sinTheta, -1.f, 1.f );
    return RadToDeg( asinf( sinTheta ) );
}

float ArcCosDeg( float cosTheta )
{
    cosTheta = Clampf( cosTheta, -1.f, 1.f );
    return RadToDeg( acosf( cosTheta ) );
}

bool CyclicLesser( float a, float b, float range )
{
    float diff = b - a;
    float modDiff = ModFloat( diff, range );
    return modDiff <= range * 0.5f;
}

float ModFloat( float a, float b )
{
    float m = fmodf( a, b );
    if( m < 0 )
        m += b;
    return m;
}

float GetAngularDisplacement( float startDeg, float endDeg )
{
    float displacement = endDeg - startDeg;
    displacement = MapDegreesToPlusMinus180( displacement );
    return displacement;
}

float TurnToward( float currentDeg, float goalDeg, float maxTurnDeg )
{
    maxTurnDeg = fabsf( maxTurnDeg );
    float displacement = GetAngularDisplacement( currentDeg, goalDeg );
    if( fabsf( displacement ) < maxTurnDeg )
    {
        return displacement + currentDeg;
    }
    else
    {
        return ( Sign( displacement ) * maxTurnDeg ) + currentDeg;
    }

}

float MapDegreesTo360( float degrees )
{
    while( degrees > 360 )
        degrees -= 360;
    while( degrees < 0 )
        degrees += 360;
    return degrees;
}

float MapDegreesToPlusMinus180( float degrees )
{
    while( degrees > 180 )
        degrees -= 360;
    while( degrees < -180 )
        degrees += 360;
    return degrees;
}

float Minf( float a, float b )
{
    return ( a < b ) ? a : b;
}

float Maxf( float a, float b )
{
    return ( a > b ) ? a : b;
}


int RoundToInt( float inValue )
{
    float floorValue = floor( inValue );
    if( inValue - floorValue >= 0.5f )
        return (int) ( floorValue + 1 );
    else
        return (int) floorValue;
    // return (int) std::roundf( inValue ); // this does not pass test
}

int FloorToInt( float inValue )
{
    // floorf behaves well for negatives
    return (int) floorf( inValue );
}

float GetDecimalPart( float value )
{
    return value - (long) value;
}

float GetDecimalPartPositive( float value )
{
    float val = GetDecimalPart( value );
    if( val < 0.f )
        val += 1;
    return val;
}

float SnapToMultiplesOf( float value, float multiplesOf )
{
    float quotient = value / multiplesOf;
    quotient = roundf( quotient );
    return quotient * multiplesOf;
}

Vec2 SnapToMultiplesOf( const Vec2& value, float multiplesOf )
{
    return Vec2( SnapToMultiplesOf( value.x, multiplesOf ),
                 SnapToMultiplesOf( value.y, multiplesOf ) );
}

Vec3 SnapToMultiplesOf( const Vec3& value, float multiplesOf )
{
    return Vec3( SnapToMultiplesOf( value.x, multiplesOf ),
                 SnapToMultiplesOf( value.y, multiplesOf ),
                 SnapToMultiplesOf( value.z, multiplesOf ) );
}



int ClampInt( int inValue, int min, int max )
{
    inValue = ( inValue < min ) ? min : inValue;
    inValue = ( inValue > max ) ? max : inValue;
    return inValue;
}

float Clampf( float inValue, float min, float max )
{
    inValue = ( inValue < min ) ? min : inValue;
    inValue = ( inValue > max ) ? max : inValue;
    return inValue;
}

float Clampf01( float inValue )
{
    return Clampf( inValue, 0.f, 1.f );
}

float ClampfNegativeOneToOne( float inValue )
{
    return Clampf( inValue, -1.f, 1.f );
}

float GetFractionInRange( float inValue, float rangeStart, float rangeEnd )
{
    float rangeSpan = rangeEnd - rangeStart;
    if( rangeSpan == 0.f )
        return 0.f;
    float valueInSpan = inValue - rangeStart;
    return valueInSpan / rangeSpan;
}

float RangeMapFloat( float inValue, float inStart, float inEnd, float outStart, float outEnd )
{
    if( inStart == inEnd )
    {
        return ( outStart + outEnd ) * 0.5f;
    }
    float inRange = inEnd - inStart;
    float outRange = outEnd - outStart;
    float inRelativeToStart = inValue - inStart;
    float fractionIntoRange = inRelativeToStart / inRange;
    float outRelativeToStart = fractionIntoRange * outRange;
    return outRelativeToStart + outStart;
}

float RangeMapFloat01( float inValue, float inStart, float inEnd )
{
    return RangeMapFloat( inValue, inStart, inEnd, 0.f, 1.f );
}

int RangeMapInt( int inVal, int inStart, int inEnd, int outStart, int outEnd )
{
    float outVal = RangeMapFloat( (float) inVal, (float) inStart, (float) inEnd, (float) outStart, (float) outEnd );
    return RoundToInt( outVal );
}

float Sign( float x )
{
    if( x < 0.f )
        return -1.f;
    if( x > 0.f )
        return 1.f;
    return 0.f;

}

int Sign( int x )
{
    if( x < 0 )
        return -1;
    if( x > 0 )
        return 1;
    return 0;
}


int Lerp( int start, int end, float t )
{
    if( t >= 1.f ) //TODO: This function really needs more work, test on asteroids
        return end;
    int span = end - start;
    span = span < 0 ? span - 1 : span + 1;

    return start + FloorToInt( t * (float) span );
}

unsigned char Lerp( unsigned char start, unsigned char end, float t )
{
    int value = ClampInt( Lerp( (int) start, (int) end, t ), 0, 255 );
    return (unsigned char) value;
}



bool AreBitsSet( unsigned char bitFlags8, unsigned char flagsToCheck )
{
    return ( bitFlags8 & flagsToCheck ) == flagsToCheck;
}

bool AreBitsSet( unsigned int bitFlags32, unsigned int flagsToCheck )
{
    return ( bitFlags32 & flagsToCheck ) == flagsToCheck;
}

void SetBits( unsigned char& bitFlags8, unsigned char flagsToSet )
{
    bitFlags8 |= flagsToSet;
}

void SetBits( unsigned int& bitFlags32, unsigned int flagsToSet )
{
    bitFlags32 |= flagsToSet;
}

void ClearBits( unsigned char& bitFlags8, unsigned char flagToClear )
{
    bitFlags8 &= ~flagToClear;
}

void ClearBits( unsigned int& bitFlags32, unsigned int flagToClear )
{
    bitFlags32 &= ~flagToClear;
}

float Easing::SmoothStart2( float t )
{
    return t * t;
}

float Easing::SmoothStart3( float t )
{
    return t * t * t;
}

float Easing::SmoothStart4( float t )
{
    return t * t * t * t;
}

float Easing::SmoothStop2( float t )
{
    return 1.f - ( 1.f - t ) * ( 1.f - t );
}

float Easing::SmoothStop3( float t )
{
    float t1 = 1.f - t;
    return 1.f - t1 * t1 * t1;
}

float Easing::SmoothStop4( float t )
{
    float t1 = 1.f - t;
    return 1.f - t1 * t1 * t1 * t1;
}

float Easing::SmoothStep3( float t )
{
    return Lerp( SmoothStart3( t ), SmoothStop3( t ), t );
}

float Easing::Bell( float t )
{
    return 4.f * t * ( 1.f - t );
}

float Easing::Bell2( float t )
{
    float t1 = Bell( t );
    return t1 * t1;
}

float Easing::Bell3( float t )
{
    float t1 = Bell( t );
    return t1 * t1 * t1;
}

float Easing::Bell4( float t )
{
    float t1 = Bell( t );
    return t1 * t1 * t1 * t1;
}


float Easing::BackStart( float t )
{
    float overShoot = 1.5f;
    return t * ( ( overShoot + 1.f ) * t - overShoot );
}

float Easing::BackStop( float t )
{
    return 1.f - BackStart( 1.f - t );
}

float Easing::BackStartStop( float t )
{
    return Lerp( BackStart( t ), BackStop( t ), t );
}

float Easing::BounceStart( float t )
{
    return 1.f - BounceStop( 1.f - t );
}

float Easing::BounceStop( float t )
{
    float a = 7.5625f;
    if( t < ( 1.f / 2.75f ) )
    {
        return ( a * t * t );
    }
    else if( t < ( 2.f / 2.75f ) )
    {
        t-=( 1.5f / 2.75f );
        return ( a * t * t + .75f );
    }
    else if( t < ( 2.5f / 2.75f ) )
    {
        t-=( 2.25f / 2.75f );
        return ( a * t *t + .9375f );
    }
    else
    {
        t-=( 2.625f / 2.75f );
        return (float) ( a * t *t + .984375 );
    }
}


float Easing::ElasticStart( float t )
{
    return 1.f - ElasticStop( 1.f - t );
}

float Easing::ElasticStop( float t )
{
    if( t == 0.f ) return 0.f;  if( t == 1.f ) return 1.f;
    float p = .3f;
    float s = p / 4.f;
    return  ( pow( 2.f, -10.f * t ) * sin( ( t - s )*( 2.f * (float) M_PI ) / p ) + 1.f );
}

float Easing::ElasticStartStop( float t )
{
    t *= 2.f;
    if( t == 0 ) return 0.f;  if( t == 2.f ) return 1.f;
    float p = .3f*1.5f;
    float s = p / 4.f;

    if( t < 1 )
    {
        float postFix = (float) pow( 2, 10 * ( t-=1 ) );
        return ( -.5f*( postFix* sin( ( t - s )*( 2 * (float) M_PI ) / p ) ) );
    }
    float postFix = (float) pow( 2, -10 * ( t-=1 ) );
    return postFix * sin( ( t - s )*( 2 * (float) M_PI ) / p )*.5f + 1.f;

}

float Wave::SawTooth( float t )
{
    return t - floorf( t );
}

float Wave::Square( float t )
{
    t = SawTooth( t );

    if( t < 0.5f )
        return 0;

    return 1.f;
}

float Wave::Triangle( float t )
{
    t = SawTooth( t );
    return -fabsf( 1.f - ( 2 * t ) );
}

float Wave::PeriodicSawTooth( float t, float interval, float sawToothDuration /*= 1.f */ )
{
    t = fmodf( t, interval );

    if( t < 0 )
        t += interval;

    if( t > sawToothDuration )
        return 0.f;

    t = t / sawToothDuration; //map t to [0,1]

    return SawTooth( t );
}


float Wrap::WrapFloat01( float value, WrapMode mode )
{
    switch( mode )
    {
    case CLAMP:
        return Clampf01( value );
    case REPEAT:
        return Repeat01( value ); // -0.1 goes to 0.9
    case MIRROR:
        return Mirror01( value );
    case UNBOUND:
        return value;
    default:
        return value;
    }
}


Vec2 Wrap::WrapUV01( const Vec2& uv, WrapMode mode )
{
    return Vec2( WrapFloat01( uv.u, mode ), WrapFloat01( uv.v, mode ) );
}

float Wrap::Repeat01( float value )
{
    // this is because GetDecimalPartPositive(1) is 0 and we want 1
    if( value == 1.f )
        return 1.f;
    return GetDecimalPartPositive( value );
}

float Wrap::Mirror01( float value )
{
    // the cycle is 2 if mirrored
    value = fmodf( value, 2.f );
    if( value < 0 )
        value += 2.f;

    if( value <= 1 )
        return value;

    return 2.f - value;
}
