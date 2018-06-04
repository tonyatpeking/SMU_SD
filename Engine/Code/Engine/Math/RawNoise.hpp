//-----------------------------------------------------------------------------------------------
// RawNoise.hpp
//
#pragma once


/////////////////////////////////////////////////////////////////////////////////////////////////
// SquirrelNoise4 - Squirrel's Raw Noise utilities (version 4)
//
// This code is made available under the Creative Commons attribution 3.0 license (CC-BY-3.0 US):
//	Attribution in source code comments (even closed-source/commercial code) is sufficient.
//	License summary and text available at: https://creativecommons.org/licenses/by/3.0/us/
//
// These noise functions were written by Squirrel Eiserloh as a cheap and simple substitute for
//	the [sometimes awful] bit-noise sample code functions commonly found on the web, many of which
//	are hugely biased or terribly patterned, e.g. having bits which are on (or off) 75% or even
//	100% of the time (or are WAY too overkill-and-slow for our needs, such as MD5 or SHA).
//
// Note: This is work in progress, and has not yet been tested thoroughly.  Use at your own risk.
//	Please report any bugs, issues, or bothersome cases to SquirrelEiserloh at gmail.com.
//
// The following functions are all based on a simple bit-noise hash function which returns an
//	unsigned integer containing 32 reasonably-well-scrambled bits, based on a given (signed)
//	integer input parameter (position/index) and [optional] seed.  Kind of like looking up a
//	value in an infinitely large [non-existent] table of previously rolled random numbers.
//
// These functions are deterministic and random-access / order-independent (i.e. state-free),
//	so they are particularly well-suited for use in smoothed/fractal/simplex/Perlin noise
//	functions and out-of-order (or or-demand) procedural content generation (i.e. that mountain
//	village is the same whether you generated it first or last, ahead of time or just now).
//
// The N-dimensional variations simply hash their multidimensional coordinates down to a single
//	32-bit index and then proceed as usual, so while results are not unique they should
//	(hopefully) not seem locally predictable or repetitive.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

namespace Noise

{


//-----------------------------------------------------------------------------------------------
// Raw pseudorandom noise functions (random-access / deterministic).  Basis of all other noise.
//
unsigned int Get1dUint( int index, unsigned int seed=0 );
unsigned int Get2dUint( int indexX, int indexY, unsigned int seed=0 );
unsigned int Get3dUint( int indexX, int indexY, int indexZ, unsigned int seed=0 );
unsigned int Get4dUint( int indexX, int indexY, int indexZ, int indexT, unsigned int seed=0 );

//-----------------------------------------------------------------------------------------------
// Same functions, mapped to floats in [0,1] for convenience.
//
float Get1dZeroToOne( int index, unsigned int seed=0 );
float Get2dZeroToOne( int indexX, int indexY, unsigned int seed=0 );
float Get3dZeroToOne( int indexX, int indexY, int indexZ, unsigned int seed=0 );
float Get4dZeroToOne( int indexX, int indexY, int indexZ, int indexT, unsigned int seed=0 );

//-----------------------------------------------------------------------------------------------
// Same functions, mapped to floats in [-1,1] for convenience.
//
float Get1dNegOneToOne( int index, unsigned int seed=0 );
float Get2dNegOneToOne( int indexX, int indexY, unsigned int seed=0 );
float Get3dNegOneToOne( int indexX, int indexY, int indexZ, unsigned int seed=0 );
float Get4dNegOneToOne( int indexX, int indexY, int indexZ, int indexT, unsigned int seed=0 );


/////////////////////////////////////////////////////////////////////////////////////////////////
// Simple functions inlined below
/////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
inline unsigned int Get2dUint( int indexX, int indexY, unsigned int seed )
{
    const int PRIME_NUMBER = 198491317; // Large prime number with non-boring bits
    return Get1dUint( indexX + ( PRIME_NUMBER * indexY ), seed );
}


//-----------------------------------------------------------------------------------------------
inline unsigned int Get3dUint( int indexX, int indexY, int indexZ, unsigned int seed )
{
    const int PRIME1 = 198491317; // Large prime number with non-boring bits
    const int PRIME2 = 6542989; // Large prime number with distinct and non-boring bits
    return Get1dUint( indexX + ( PRIME1 * indexY ) + ( PRIME2 * indexZ ), seed );
}


//-----------------------------------------------------------------------------------------------
inline unsigned int Get4dUint( int indexX, int indexY, int indexZ, int indexT, unsigned int seed )
{
    const int PRIME1 = 198491317; // Large prime number with non-boring bits
    const int PRIME2 = 6542989; // Large prime number with distinct and non-boring bits
    const int PRIME3 = 357239; // Large prime number with distinct and non-boring bits
    return Get1dUint( indexX + ( PRIME1 * indexY ) + ( PRIME2 * indexZ ) + ( PRIME3 * indexT ), seed );
}


//-----------------------------------------------------------------------------------------------
inline float Get1dZeroToOne( int index, unsigned int seed )
{
    const double ONE_OVER_MAX_UINT = ( 1.0 / (double) 0xFFFFFFFF );
    return (float) ( ONE_OVER_MAX_UINT * (double) Get1dUint( index, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get2dZeroToOne( int indexX, int indexY, unsigned int seed )
{
    const double ONE_OVER_MAX_UINT = ( 1.0 / (double) 0xFFFFFFFF );
    return (float) ( ONE_OVER_MAX_UINT * (double) Get2dUint( indexX, indexY, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get3dZeroToOne( int indexX, int indexY, int indexZ, unsigned int seed )
{
    const double ONE_OVER_MAX_UINT = ( 1.0 / (double) 0xFFFFFFFF );
    return (float) ( ONE_OVER_MAX_UINT * (double) Get3dUint( indexX, indexY, indexZ, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get4dZeroToOne( int indexX, int indexY, int indexZ, int indexT, unsigned int seed )
{
    const double ONE_OVER_MAX_UINT = ( 1.0 / (double) 0xFFFFFFFF );
    return (float) ( ONE_OVER_MAX_UINT * (double) Get4dUint( indexX, indexY, indexZ, indexT, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get1dNegOneToOne( int index, unsigned int seed )
{
    const double ONE_OVER_MAX_INT = ( 1.0 / (double) 0x7FFFFFFF );
    return (float) ( ONE_OVER_MAX_INT * (double) (int) Get1dUint( index, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get2dNegOneToOne( int indexX, int indexY, unsigned int seed )
{
    const double ONE_OVER_MAX_INT = ( 1.0 / (double) 0x7FFFFFFF );
    return (float) ( ONE_OVER_MAX_INT * (double) (int) Get2dUint( indexX, indexY, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get3dNegOneToOne( int indexX, int indexY, int indexZ, unsigned int seed )
{
    const double ONE_OVER_MAX_INT = ( 1.0 / (double) 0x7FFFFFFF );
    return (float) ( ONE_OVER_MAX_INT * (double) (int) Get3dUint( indexX, indexY, indexZ, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get4dNegOneToOne( int indexX, int indexY, int indexZ, int indexT, unsigned int seed )
{
    const double ONE_OVER_MAX_INT = ( 1.0 / (double) 0x7FFFFFFF );
    return (float) ( ONE_OVER_MAX_INT * (double) (int) Get4dUint( indexX, indexY, indexZ, indexT, seed ) );
}

}