#include "Engine/Math/Solver.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <cmath>


namespace Solver
{

bool Quadratic( float a, float b, float c, float& out_root1, float& out_root2 )
{
    if( a == 0.f )
    {
        if( b == 0.f )
            return false;
        out_root1 = -c / b;
        out_root2 = out_root1;
        return true;
    }

    float discriminant = b * b - 4.f * a * c;
    if( discriminant < 0.f )
        return false;

    float aInv = 1.f / a;
    float dSqrt = sqrtf( discriminant ) * 0.5f * aInv;
    float maxima = -0.5f * b * aInv;
    float r1 = maxima - dSqrt;
    float r2 = maxima + dSqrt;
    if( r1 < r2 )
    {
        out_root1 = r1;
        out_root2 = r2;
    }
    else
    {
        out_root2 = r1;
        out_root1 = r2;
    }
    return true;
}


float BinarySearch( float start, float end,
                    ::std::function<float( float t )> eval,
                    int iterations /*= 10*/, float eps )
{
    float vStart = eval( start );
    float vEnd = eval( end );

    // early out if slope is 0
    if( AlmostZero( vEnd - vStart ) )
        return ( start + end ) / 2.f;

    float vStartSign = Sign( vStart );
    float vEndSign = Sign( vEnd );

    // early out if no root
    if( vStartSign == vEndSign )
    {
        if( fabs( vStart ) < fabs( vEnd ) )
            return start;
        return end;
    }


    float tStart = start;
    float tEnd = end;
    for( int iter = 0; iter < iterations; ++iter )
    {
        float mid = ( tStart + tEnd ) / 2.f;
        float vMid = eval( mid );
        float vMidSign = Sign( vMid );
        // early out if found root
        if( AlmostZero( vMid, eps ) )
            return mid;

        if( vMidSign == vStartSign )
        {
            tStart = mid;
            vStartSign = vMidSign;
        }
        else
        {
            tEnd = mid;
            vEndSign = vMidSign;
        }
    }

    return tStart;
}



}



