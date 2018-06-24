#pragma once
#include "Engine/Core/EngineCommon.hpp"
// Forward declaration
class Vec2;
class Vec3;
class Rgba;
class AABB2;
class Range;
class Disc2;

// Stand alone functions
bool AlmostZero( float f, float eps = EPSILON );
float SnapToZero( float f );
bool AlmostEqual( const float a, const float b );
bool AlmostEqual( const Vec2& a, const Vec2& b );
bool AlmostEqual( const AABB2& a, const AABB2& b );
bool AlmostEqual( const Range& a, const Range& b );
bool AlmostEqual( const Disc2& a, const Disc2& b );

float RadToDeg( const float rad );
float DegToRad( const float deg );
float CosDeg( const float deg );
float SinDeg( const float deg );
float TanDeg( const float deg );
float Atan2Deg( float y, float x );
float ArcSinDeg( float deg );
float ArcCosDeg( float deg );


//Angle & Vector utilities
// Finds the "angular displacement" (or signed angular distance) from startDeg to endDeg.
// This function always takes the smartest/shortest way around, so going from 5 to 355 yields -10.
float GetAngularDisplacement( float startDeg, float endDeg );
// I face currentDeg and want to turn "toward" goalDeg, by up to maxTurnDeg.
// Note: must always take the shortest/smartest way around, and will never overshoot.
// returns new facing, NOT the delta degrees
float TurnToward( float currentDeg, float goalDeg, float maxTurnDeg );
float MapDegreesTo360( float degrees );
float MapDegreesToPlusMinus180( float degrees );

float Minf( float a, float b );
float Maxf( float a, float b );

//ints and floats
int RoundToInt( float inValue );			// 0.5 rounds up to 1; -0.5 rounds up to 0
int FloorToInt( float inValue );			// 0.5 floors to 0; -0.5 floors to -1
float GetDecimalPart( float value );
float GetDecimalPartPositive( float value );
float SnapToMultiplesOf( float value, float multiplesOf );
Vec2 SnapToMultiplesOf( const Vec2& value, float multiplesOf );
Vec3 SnapToMultiplesOf( const Vec3& value, float multiplesOf );

int ClampInt( int inValue, int min, int max );
float Clampf( float inValue, float min, float max );
float Clampf01( float inValue );
float ClampfNegativeOneToOne( float inValue );

// For a value in [inStart,inEnd], finds the corresponding value in [outStart,outEnd].
// Note: must correctly interpolate and extrapolate, and handle all special cases.
float RangeMapFloat( float inValue, float inStart, float inEnd, float outStart, float outEnd );
float RangeMapFloat01( float inValue, float inStart, float inEnd );
int RangeMapInt( int inVal, int inStart, int inEnd, int outStart, int outEnd );
// Range map for anything that has component based for all == + - * / operators
// Not safe, div by 0 if inStart and inEnd have components that are equal
template<typename T>
T RangeMap( const T& inValue, const T& inStart, const T& inEnd, const T& outStart, const T& outEnd )
{
    if( inStart == inEnd )
    {
        return ( outStart + outEnd ) * 0.5f;
    }
    T inRange = inEnd - inStart;
    T outRange = outEnd - outStart;
    T inRelativeToStart = inValue - inStart;
    T fractionIntoRange = inRelativeToStart / inRange;
    T outRelativeToStart = fractionIntoRange * outRange;
    return outRelativeToStart + outStart;
}

float Sign( float x );
int Sign( int x );

// Lerp functions
// Finds the % (as a fraction) of inValue in [rangeStart,rangeEnd].
// For example, 3 is 25% (0.25) of the way through the range [2,6].
float GetFractionInRange( float inValue, float rangeStart, float rangeEnd );


// Lerp for anything that has component based for + - operators and * float operator
template<typename T>
T Lerp( const T& start, const T& end, float t )
{
    return start + ( end - start ) * t;
}
int Lerp( int start, int end, float t );
unsigned char Lerp( unsigned char start, unsigned char end, float t );

// Is in Range
template<typename T>
bool IsInRange( const T& value, const T& rangeMin, const T& rangeMax )
{
    return ( rangeMin <= value ) && ( value <= rangeMax );
}


// Bit flag utilities
bool AreBitsSet( unsigned char bitFlags8, unsigned char flagsToCheck );
bool AreBitsSet( unsigned int bitFlags32, unsigned int flagsToCheck );
void SetBits( unsigned char& bitFlags8, unsigned char flagsToSet );
void SetBits( unsigned int& bitFlags32, unsigned int flagsToSet );
void ClearBits( unsigned char& bitFlags8, unsigned char flagToClear );
void ClearBits( unsigned int& bitFlags32, unsigned int flagToClear );

// Easing functions
namespace Easing
{

float SmoothStart2( float t ); // 2nd-degree smooth start (a.k.a. "quadratic ease in")
float SmoothStart3( float t ); // 3rd-degree smooth start (a.k.a. "cubic ease in")
float SmoothStart4( float t ); // 4th-degree smooth start (a.k.a. "quartic ease in")
float SmoothStop2( float t ); // 2nd-degree smooth start (a.k.a. "quadratic ease out")
float SmoothStop3( float t ); // 3rd-degree smooth start (a.k.a. "cubic ease out")
float SmoothStop4( float t ); // 4th-degree smooth start (a.k.a. "quartic ease out")
float SmoothStep3( float t ); // 3rd-degree smooth start/stop (a.k.a. "smoothstep")
float Bell( float t ); //just a quadratic bell
float Bell2( float t );//each one is smoother then the previous
float Bell3( float t );
float Bell4( float t );


//reference: https://github.com/jesusgollonet/ofpennereasing/tree/master/PennerEasing
// http://robertpenner.com/easing/
float BackStart( float t );
float BackStop( float t );
float BackStartStop( float t );
float BounceStart( float t );
float BounceStop( float t );
float ElasticStart( float t );
float ElasticStop( float t );
float ElasticStartStop( float t );

}

// Basic waves, all have range of [0,1], period of 1.f and start at 0.f
namespace Wave
{
float   SawTooth( float t );
float   Square( float t );
float   Triangle( float t );

// Composition waves
float PeriodicSawTooth( float t, float interval, float sawToothDuration = 1.f );  //periodically makes a sawtooth

}



namespace Wrap
{
enum WrapMode
{
    CLAMP,
    REPEAT,
    MIRROR,
    UNBOUND
};

float WrapFloat01( float value, WrapMode mode );
Vec2 WrapUV01( const Vec2& uv, WrapMode mode );

float Repeat01( float value );
float Mirror01( float value );

}


