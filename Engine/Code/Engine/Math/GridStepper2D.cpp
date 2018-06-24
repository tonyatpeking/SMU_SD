#include "Engine/Math/GridStepper2D.hpp"
#include "Engine/Math/MathUtils.hpp"


GridStepper2D::GridStepper2D( const Ray2& ray, const Vec2& cellSize, const Vec2& origin /*= Vec2::ZEROS */ )
    : m_ray( ray )
    , m_cellSize( cellSize )
    , m_origin( origin )
{
    Init();
}

Vec2 GridStepper2D::GetCurrentPos()
{
    return m_ray.Evaluate( m_currentT );
}

float GridStepper2D::GetCurrentT()
{
    return m_currentT;
}

IVec2 GridStepper2D::GetCurrentCellCoords()
{
    return m_currentCoords;
}

void GridStepper2D::Step()
{
    float tToNextCol = m_prevColT + m_tToJumpCol;
    float tToNextRow = m_prevRowT + m_tToJumpRow;
    if( tToNextCol < tToNextRow )
    {
        m_currentT = tToNextCol;
        m_prevColT = tToNextCol;
        m_currentCoords.x += m_signRayXDir;
    }
    else
    {
        m_currentT = tToNextRow;
        m_prevRowT = tToNextRow;
        m_currentCoords.y += m_signRayYDir;
    }
}

void GridStepper2D::Init()
{
    m_signRayXDir = (int) Sign( m_ray.direction.x );
    m_signRayYDir = (int) Sign( m_ray.direction.y );
    CalcInitCoords();
    CalcTToJump();
    CalcInitPrevTs();
}

void GridStepper2D::CalcInitCoords()
{
    Vec2 relativePos = m_ray.start - m_origin;
    Vec2 coordsF = relativePos / m_cellSize;
    coordsF.SnapToZero();

    m_currentCoords = IVec2( FloorToInt( coordsF.x ), FloorToInt( coordsF.y ) );
}

void GridStepper2D::CalcTToJump()
{
    m_tToJumpCol = fabs( m_cellSize.x / m_ray.direction.x );
    m_tToJumpRow = fabs( m_cellSize.y / m_ray.direction.y );
}


void GridStepper2D::CalcInitPrevTs()
{
    // bottom left corner
    Vec2 cellPos = Vec2( m_currentCoords ) * m_cellSize;
    float xDist = m_ray.start.x - cellPos.x;
    float yDist = m_ray.start.y - cellPos.y;
    if( AlmostZero( xDist ) )
        xDist = 0.f;
    if( AlmostZero( yDist ) )
        yDist = 0.f;

    if( m_signRayXDir < 0 && xDist != 0 )
        xDist = cellPos.x + m_cellSize.x - m_ray.start.x;
    if( m_signRayYDir < 0 && yDist != 0 )
        yDist = cellPos.y + m_cellSize.y - m_ray.start.y;

    // xDist and yDist are always positive or zero
    // the initial T's are always zero or negative

    m_prevColT = -xDist / m_ray.direction.x;
    m_prevRowT = -yDist / m_ray.direction.y;



    if( std::isnan( m_prevColT ) || std::isinf( m_prevColT ) )
        m_prevColT = 0;
    if( std::isnan( m_prevRowT ) || std::isinf( m_prevRowT ) )
        m_prevRowT = 0;
}

