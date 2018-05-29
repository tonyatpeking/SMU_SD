#include "Engine/Math/CubicSpline.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"

CubicSpline2D::CubicSpline2D( const Vec2* positionsArray, int numPoints, const Vec2* velocitiesArray/*=nullptr */ )
{
    AppendPoints( positionsArray, numPoints, velocitiesArray );
}

void CubicSpline2D::AppendPoint( const Vec2& position, const Vec2& velocity/*=Vec2::ZEROS */ )
{
    m_positions.push_back( position );
    m_velocities.push_back( velocity );
}

void CubicSpline2D::AppendPoints( const Vec2* positionsArray, int numPoints, const Vec2* velocitiesArray/*=nullptr */ )
{
    m_positions.reserve( m_positions.size() + numPoints );
    m_velocities.reserve( m_velocities.size() + numPoints );
    for( int pointIdx = 0; pointIdx < numPoints; ++pointIdx )
    {
        m_positions.push_back( positionsArray[pointIdx] );
        if( velocitiesArray )
            m_velocities.push_back( velocitiesArray[pointIdx] );
        else
            m_velocities.push_back( Vec2::ZEROS );
    }
}

void CubicSpline2D::InsertPoint( int insertBeforeIndex, const Vec2& position, const Vec2& velocity/*=Vec2::ZEROS */ )
{
    m_positions.insert( m_positions.begin() + insertBeforeIndex, position );
    m_velocities.insert( m_velocities.begin() + insertBeforeIndex, velocity );
}

void CubicSpline2D::RemovePoint( int pointIndex )
{
    m_positions.erase( m_positions.begin() + pointIndex );
    m_velocities.erase( m_velocities.begin() + pointIndex );
}

void CubicSpline2D::RemoveAllPoints()
{
    m_positions.clear();
    m_velocities.clear();
}

void CubicSpline2D::SetPoint( int pointIndex, const Vec2& newPosition, const Vec2& newVelocity )
{
    m_positions[pointIndex] = newPosition;
    m_velocities[pointIndex] = newVelocity;
}

void CubicSpline2D::SetPosition( int pointIndex, const Vec2& newPosition )
{
    m_positions[pointIndex] = newPosition;
}

void CubicSpline2D::SetVelocity( int pointIndex, const Vec2& newVelocity )
{
    m_velocities[pointIndex] = newVelocity;
}

void CubicSpline2D::SetCardinalVelocities( float tension/*=0.f*/, const Vec2& startVelocity/*=Vec2::ZEROS*/, const Vec2& endVelocity/*=Vec2::ZEROS */ )
{
    int numPoints = (int) m_positions.size();
    if( numPoints == 0 )
        return;
    m_velocities.resize( numPoints );
    m_velocities[0] = startVelocity;
    m_velocities[numPoints - 1] = endVelocity;
    float velocityMultiplier =  0.5f * ( 1 - tension );
    for( int pointIdx = 0; pointIdx <= numPoints - 3; ++pointIdx )
    {
        m_velocities[pointIdx + 1] = ( m_positions[pointIdx + 2] - m_positions[pointIdx] ) * velocityMultiplier;
    }
}

const Vec2 CubicSpline2D::GetPosition( int pointIndex ) const
{
    return m_positions[pointIndex];
}

const Vec2 CubicSpline2D::GetVelocity( int pointIndex ) const
{
    return m_velocities[pointIndex];
}

int CubicSpline2D::GetPositions( std::vector<Vec2>& out_positions ) const
{
    out_positions = m_positions;
    return (int) m_positions.size();
}

int CubicSpline2D::GetVelocities( std::vector<Vec2>& out_velocities ) const 
{
    out_velocities = m_velocities;
    return (int) m_velocities.size();
}

Vec2 CubicSpline2D::EvaluateAtCumulativeParametric( float t ) const
{
    int segment = (int) t;
    segment = ClampInt( segment, 0, (int) m_positions.size() - 2 );
    float localT = t - (float) segment;

    const Vec2& startPos = m_positions[segment];
    const Vec2& startVel = m_velocities[segment];
    const Vec2& endPos = m_positions[segment + 1];
    const Vec2& endVel = m_velocities[segment + 1];
    return EvaluateCubicHermite( startPos, startVel, endPos, endVel, localT );
}

Vec2 CubicSpline2D::EvaluateAtNormalizedParametric( float t ) const
{

    float cumulativeT = t * (float) ( m_positions.size() - 1 );

    return EvaluateAtCumulativeParametric( cumulativeT );

}
