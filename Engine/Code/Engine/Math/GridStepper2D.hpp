#pragma once

#include "Engine/Math/Ray2.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/Vec2.hpp"

class GridStepper2D
{
    // see http://playtechs.blogspot.com/2007/03/raytracing-on-grid.html
public:
    // Grid cells are bottom left aligned
    GridStepper2D( const Ray2& ray, const Vec2& cellSize, const Vec2& origin = Vec2::ZEROS );
    ~GridStepper2D() {};
    Vec2 GetCurrentPos();
    float GetCurrentT();
    IVec2 GetCurrentCellCoords();
    void Step();

private:

    void Init();
    void CalcInitCoords();
    void CalcTToJump();
    // if starts on a grid line,
    void CalcInitPrevTs();


    Ray2 m_ray;
    Vec2 m_cellSize;
    Vec2 m_origin;

    // cached at start
    int m_signRayXDir = 0;
    int m_signRayYDir = 0;

    // cached at start
    // These can be inf, but always positive
    float m_tToJumpCol = 0;
    float m_tToJumpRow = 0;

    // updated each step
    float m_currentT = 0;
    IVec2 m_currentCoords;

    // only updated if jumped a col or row
    float m_prevColT = 0;
    float m_prevRowT = 0;
};
