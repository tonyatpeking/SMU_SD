#pragma once
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Core/SmartEnum.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Types.hpp"
#include <string>
#include <vector>
#include <functional>

class Mesh;

class BarGraphBuilder : public MeshBuilder
{
public:
    BarGraphBuilder( const AABB2& bounds );

    ~BarGraphBuilder() {};

    void Finalize();

    std::vector<float>& GetDataRef();

    void SetBarWidth( float barWidth ) { m_barWidth = barWidth; };

    void SetBarWidthFromCount( float fitThisManyBars );

    float GetBarWidth() { return m_barWidth; };


    // 0 for left, 1 for right, 0.5 for center
    void SetAlignment( float alignment ) { m_barAlignment = alignment; };

    uint GetDataCount() { return (uint) m_data.size(); };

    float GetMaxData();

    float GetMinData();


    // if scaleToMaxData is true, m_maxYRange will effectively be max( m_maxYRange, m_maxData)
    void SetYAxisRange( float minY, float maxY,
                        bool scaleToMaxData = true, bool scaleToMinData = false );


    // Bar Coloring, if this is set, will be used in instead of m_barColor
    // example:
    //     m_dataToColor = []( float value, int barIdx, float minData, float maxData )
    //     {
    //         float t = RangeMapFloat01( value, minData, maxData );
    //         Rgba color = Lerp( Rgba::GREEN, Rgba::RED, t );
    //         return color;
    //     };

    // minData and maxData are actual mins and maxs of the data
    void SetBarColoringFunction(
        std::function<Rgba( float value, int barIdx, float minData, float maxData )> dataToColor )
    {
        m_dataToColor = dataToColor;
    };

    void SetBarColor( const Rgba& color ) { m_barColor = color; };


private:


    AABB2 m_bounds;
    float m_barWidth = 1.f;
    float m_barAlignment = 0.f; // 0 for left, 1 for right, 0.5 for center

    float m_minYRange = 0.f;
    float m_maxYRange = 1.f;
    bool m_scaleToMaxData = true;
    bool m_scaleToMinData = false;

    float m_maxData = -INFINITY;
    float m_minData = INFINITY;
    std::vector<float> m_data;


    std::function<Rgba( float, int, float, float )> m_dataToColor = nullptr;

    Rgba m_barColor = Rgba::WHITE;
};
