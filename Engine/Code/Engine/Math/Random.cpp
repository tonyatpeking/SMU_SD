#include <stdlib.h>
#include <cmath>
#include <time.h>
#include <algorithm>

#include "Engine/Math/Random.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Math/Vec3.hpp"

namespace Random
{

int g_seed;

void InitSeed()
{
    //g_seed = static_cast<unsigned int>( time( NULL ) );
    g_seed = (int) ( TimeUtils::GetCurrentTimeSeconds() * 100.f );
    srand( g_seed );
}

void SetSeed( unsigned int seed )
{
    g_seed = seed;
    srand( seed );
    rand();
    rand();
    rand();
    rand();
    rand();
}

bool CheckChance( float chanceForSuccess )
{
    return FloatZeroToOne() < chanceForSuccess;
}

float FloatZeroToOne()
{
    return static_cast<float>( rand() ) * ( 1.f / static_cast<float>( RAND_MAX ) );
}

float FloatMinusOneToOne()
{
    return FloatInRange( -1.f, 1.f );
}

float FloatInRange( float minInclusive, float maxInclusive )
{
    return FloatZeroToOne() * ( maxInclusive - minInclusive ) + minInclusive;
}

float RotationDegrees()
{
    return FloatInRange( 0.f, 360.f );
}

Vec2 Direction()
{
    float dirDegrees = RotationDegrees();
    return Vec2::MakeDirectionAtDegrees( dirDegrees );
}

Vec2 PointInCircle( float radius )
{
    return PointInCircle01() * radius;
}

Vec2 PointInCircle01()
{
    float x;
    float y;
    do
    {
        x = Random::FloatMinusOneToOne();
        y = Random::FloatMinusOneToOne();
    } while( ( x * x ) + ( y * y ) > 1.f );
    return Vec2( x, y );
}

Vec2 Vec2InRange( const Vec2& min, const Vec2& max )
{
    return Vec2( FloatInRange( min.x, max.x ), FloatInRange( min.y, max.y ) );
}



Vec3 Vec3InRange( const Vec3& min, const Vec3& max )
{
    return Vec3( FloatInRange( min.x, max.x ),
                 FloatInRange( min.y, max.y ),
                 FloatInRange( min.z, max.z ) );
}

Rgba Color()
{
    return Rgba( Char(), Char(), Char(), 255 );
}

Rgba ColorInRange( const Rgba& colorA, const Rgba& colorB )
{
    return Rgba(
        CharInRange( colorA.r, colorB.r ),
        CharInRange( colorA.g, colorB.g ),
        CharInRange( colorA.b, colorB.b ),
        CharInRange( colorA.a, colorB.a ) );
}

void UniqueValuesInRange( int numOfValues, int minInclusive, int maxInclusive, vector<int>& out_uniqueValues )
{
    //#OPTIMIZE very inefficient, should probably take one out of the list and update the list every time.
    out_uniqueValues.clear();
    int spanOfValues = maxInclusive - minInclusive + 1;
    if( numOfValues > spanOfValues )
        numOfValues = spanOfValues;
    out_uniqueValues.reserve( numOfValues );
    while( numOfValues > 0 )
    {
        int randValue = Random::IntInRange( minInclusive, maxInclusive );
        if( std::find( out_uniqueValues.begin(), out_uniqueValues.end(), randValue ) != out_uniqueValues.end() )
        {
            ;
        }
        else
        {
            out_uniqueValues.push_back( randValue );
            --numOfValues;
        }
    }
}

int IntInRange( int minInclusive, int maxInclusive )
{
    if( maxInclusive < minInclusive )
    {
        int temp = minInclusive;
        minInclusive = maxInclusive;
        maxInclusive = temp;
    }
    return rand() % ( maxInclusive - minInclusive + 1 ) + minInclusive;
}

int IntLessThan( int maxNotInclusive )
{
    if( maxNotInclusive == 0 )
        return -1;
    return rand() % maxNotInclusive;
}


unsigned char Char()
{
    return (unsigned char) IntInRange( 0, 255 );
}

unsigned char CharInRange( unsigned char minInclusive, unsigned char maxInclusive )
{
    return (unsigned char) IntInRange( minInclusive, maxInclusive );
}

}