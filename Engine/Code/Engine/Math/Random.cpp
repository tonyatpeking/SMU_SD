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


Random* Random::Default()
{
    static Random* s_default = nullptr;
    if( !s_default )
        s_default = new Random();
    return s_default;
}

Random::Random()
{
    SetSeed( MakeNonDeterministic() );
}


Random::Random( uint seed )
{
    SetSeed( seed );
}

uint Random::MakeNonDeterministic()
{
    std::random_device rd;
    return rd();
}

void Random::SetSeed( uint seed )
{
    m_seed = seed;
    m_randEngine = std::default_random_engine( seed );
}

uint Random::GetSeed() const
{
    return m_seed;
}


bool Random::CheckChance( float chanceForSuccess )
{
    return FloatZeroToOne() < chanceForSuccess;
}

float Random::FloatZeroToOne()
{
    return FloatInRange( 0.f, 1.f );
}

float Random::FloatMinusOneToOne()
{
    return FloatInRange( -1.f, 1.f );
}

float Random::FloatInRange( float minInclusive, float maxNonInclusive )
{
    std::uniform_real_distribution<float> floatDist( minInclusive, maxNonInclusive );
    return floatDist( m_randEngine );
}

float Random::RotationDegrees()
{
    return FloatInRange( 0.f, 360.f );
}

Vec2 Random::Direction()
{
    float dirDegrees = RotationDegrees();
    return Vec2::MakeDirectionAtDegrees( dirDegrees );
}

Vec2 Random::PointInCircle( float radius )
{
    return PointInCircle01() * radius;
}

Vec2 Random::PointInCircle01()
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

Vec2 Random::Vec2InRange( const Vec2& min, const Vec2& max )
{
    return Vec2( FloatInRange( min.x, max.x ), FloatInRange( min.y, max.y ) );
}



Vec3 Random::Vec3InRange( const Vec3& min, const Vec3& max )
{
    return Vec3( FloatInRange( min.x, max.x ),
                 FloatInRange( min.y, max.y ),
                 FloatInRange( min.z, max.z ) );
}

Rgba Random::Color()
{
    return Rgba( Char(), Char(), Char(), 255 );
}

Rgba Random::ColorInRange( const Rgba& colorA, const Rgba& colorB )
{
    return Rgba(
        CharInRange( colorA.r, colorB.r ),
        CharInRange( colorA.g, colorB.g ),
        CharInRange( colorA.b, colorB.b ),
        CharInRange( colorA.a, colorB.a ) );
}

void Random::UniqueValuesInRange( int numOfValues, int minInclusive, int maxInclusive, vector<int>& out_uniqueValues )
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

int Random::IntInRange( int minInclusive, int maxInclusive )
{
    if( maxInclusive < minInclusive )
    {
        int temp = minInclusive;
        minInclusive = maxInclusive;
        maxInclusive = temp;
    }
    std::uniform_int_distribution<int> dist( minInclusive, maxInclusive );
    return dist( m_randEngine );
}

int Random::PositiveIntLessThan( int maxNotInclusive )
{
    if( maxNotInclusive < 0 )
        return 0;
    return IntInRange( 0, maxNotInclusive );
}


unsigned char Random::Char()
{
    return (unsigned char) IntInRange( 0, 255 );
}

unsigned char Random::CharInRange( unsigned char minInclusive, unsigned char maxInclusive )
{
    return (unsigned char) IntInRange( minInclusive, maxInclusive );
}

