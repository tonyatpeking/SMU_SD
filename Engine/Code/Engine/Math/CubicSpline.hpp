#pragma once
#include <vector>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////
// Standalone curve utility functions
//
// Note: Quadratic Bezier functions (only) specify an actual midpoint, not a Bezier guide point
/////////////////////////////////////////////////////////////////////////////////////////////////


template< typename T >
T EvaluateQuadraticBezier( const T& startPos, const T& actualCurveMidPos, const T& endPos, float t )
{
    T startEndMid = ( startPos + endPos ) * 0.5f;
    T guidePos = 2.f * actualCurveMidPos - startEndMid; // A + 2 * ( B - A ) = 2 * B - A
    T lerp1 = Lerp( startPos, guidePos, t );
    T lerp2 = Lerp( guidePos, endPos, t );
    return Lerp( lerp1, lerp2, t );
}

template< typename T >
T EvaluateCubicBezier( const T& startPos, const T& guidePos1, const T& guidePos2, const T& endPos, float t )
{
    //lerp1_1 means Tier 1 point 1
    T lerp1_1 = Lerp( startPos, guidePos1, t );
    T lerp1_2 = Lerp( guidePos1, guidePos2, t );
    T lerp1_3 = Lerp( guidePos2, endPos, t );

    T lerp2_1 = Lerp( lerp1_1, lerp1_2, t );
    T lerp2_2 = Lerp( lerp1_2, lerp1_3, t );

    return Lerp( lerp2_1, lerp2_2, t );
}

template< typename T >
T EvaluateCubicHermite( const T& startPos, const T& startVel, const T& endPos, const T& endVel, float t )
{
    T guidePos1 = startPos + startVel * 0.333333333333333f;
    T guidePos2 = endPos - endVel *  0.333333333333333f;
    return EvaluateCubicBezier( startPos, guidePos1, guidePos2, endPos, t );
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// CubicSpline2D
//
// Cubic Hermite/Bezier spline of Vec2 positions / velocities
/////////////////////////////////////////////////////////////////////////////////////////////////
class CubicSpline2D
{
public:
    CubicSpline2D() {}
    explicit CubicSpline2D( const Vec2* positionsArray, int numPoints, const Vec2* velocitiesArray=nullptr );
    ~CubicSpline2D() {}

    // Mutators
    void AppendPoint( const Vec2& position, const Vec2& velocity=Vec2::ZEROS );
    void AppendPoints( const Vec2* positionsArray, int numPoints, const Vec2* velocitiesArray=nullptr );
    void InsertPoint( int insertBeforeIndex, const Vec2& position, const Vec2& velocity=Vec2::ZEROS );
    void RemovePoint( int pointIndex );
    void RemoveAllPoints();
    void SetPoint( int pointIndex, const Vec2& newPosition, const Vec2& newVelocity );
    void SetPosition( int pointIndex, const Vec2& newPosition );
    void SetVelocity( int pointIndex, const Vec2& newVelocity );
    void SetCardinalVelocities( float tension=0.f, const Vec2& startVelocity=Vec2::ZEROS, const Vec2& endVelocity=Vec2::ZEROS );

    // Accessors
    int	GetNumPoints() const { return (int) m_positions.size(); }
    const Vec2 GetPosition( int pointIndex ) const;
    const Vec2 GetVelocity( int pointIndex ) const;
    int	GetPositions( vector<Vec2>& out_positions ) const;
    int GetVelocities( vector<Vec2>& out_velocities ) const;
    Vec2 EvaluateAtCumulativeParametric( float t ) const;
    Vec2 EvaluateAtNormalizedParametric( float t ) const;

protected:
    vector<Vec2> m_positions;
    vector<Vec2> m_velocities;
};



