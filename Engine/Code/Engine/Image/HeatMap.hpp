#pragma once

#include <vector>

#include "Engine/Math/IVec2.hpp"
#include "Engine/Core/Types.hpp"

template<typename T>
class HeatMap
{
public:
    HeatMap() {};
    HeatMap( const IVec2& dimensions, T initialHeatValue )
        : m_dimensions( dimensions )
    {
        m_heatPerGridCell.assign( dimensions.x * dimensions.y, initialHeatValue );
    }

    void SetHeat( int idx, T heat ) { m_heatPerGridCell[idx] = heat; }
    void AddHeat( int idx, T heatToAdd ) { m_heatPerGridCell[idx] += heatToAdd; }
    T GetHeat( int idx ) { return m_heatPerGridCell[idx]; }

    void SetHeat( const IVec2& cellCoords, T heat )
    {
        int cellIdx = CellCoordsToIdx( cellCoords );
        m_heatPerGridCell[cellIdx] = heat;
    }

    void AddHeat( const IVec2& cellCoords, T heatToAdd )
    {
        int cellIdx = CellCoordsToIdx( cellCoords );
        m_heatPerGridCell[cellIdx] += heatToAdd;
    }

    T GetHeat( const IVec2& cellCoords ) const
    {
        int cellIdx = CellCoordsToIdx( cellCoords );
        return m_heatPerGridCell[cellIdx];
    }

    IVec2 GetDimensions() const { return m_dimensions; }
    uint GetSize() const { return m_heatPerGridCell.size(); }

private:
    IVec2 m_dimensions;
    int CellCoordsToIdx( const IVec2& cellCoords ) const
    {
        return IVec2::CoordsToIndex( cellCoords, m_dimensions );
    };
    std::vector<T> m_heatPerGridCell;
};