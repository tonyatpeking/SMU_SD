#pragma once

#include "Engine/Core/EngineCommonH.hpp"

class Rgba;
class Vec2;
class Vec3;

namespace Random
{

extern int g_seed;

void InitSeed();
void SetSeed( unsigned int seed );

bool CheckChance( float chanceForSuccess );	// If 0.27f passed, returns true 27% of the time

float FloatZeroToOne();	// Gives floats in [0.0f,1.0f]
float FloatMinusOneToOne();	// Gives floats in [-1.f,1.f]
float FloatInRange( float minInclusive, float maxInclusive );	// Gives floats in [min,max]

int IntInRange( int minInclusive, int maxInclusive );	// Gives integers in [min,max]
int IntLessThan( int maxNotInclusive );	// Gives integers in [0,max-1]
unsigned char Char();
unsigned char CharInRange( unsigned char minInclusive, unsigned char maxInclusive);

float RotationDegrees(); // Gives floats in [0.0f,360.0f]
Vec2 Direction();

Vec2 PointInCircle( float radius );
Vec2 PointInCircle01();
Vec2 Vec2InRange( const Vec2& min, const Vec2& max );

Vec3 Vec3InRange( const Vec3& min, const Vec3& max );

Rgba Color();
Rgba ColorInRange( const Rgba& colorA, const Rgba& colorB );

// collections
void UniqueValuesInRange( int numOfValues, int minInclusive, int maxInclusive,
                     vector<int>& out_uniqueValues);

}


