#include "Engine/Renderer/BarGraphBuilder.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <cmath>

BarGraphBuilder::BarGraphBuilder( const AABB2& bounds )
    : m_bounds( bounds )
{

}

void BarGraphBuilder::Finalize()
{


    BeginSubMesh();

    uint barCount = (uint) m_data.size();
    uint vertCount = 4 * barCount;
    uint indexCount = 6 * barCount;
    Reserve( vertCount, indexCount );

    // calculate alignment and padding
    float boundsWidth = m_bounds.GetWidth();
    float boundsHeight = m_bounds.GetHeight();
    float totalPadding = boundsWidth - m_barWidth * barCount;
    float paddingLeft = m_barAlignment * totalPadding;

    float minData = GetMinData();
    float maxData = GetMaxData();
    Vec2 startBottomLeft = m_bounds.mins;
    startBottomLeft.x += paddingLeft;

    // calculate the min and max range for the Y axis
    float minYRange = m_minYRange;
    if( m_scaleToMinData )
        minYRange = Minf( minYRange, minData );
    float maxYRange = m_maxYRange;
    if( m_scaleToMaxData )
        maxYRange = Maxf( maxYRange, maxData );


    for( int barIdx = 0; barIdx < m_data.size(); ++barIdx )
    {
        float barData = m_data[barIdx];
        Vec2 barMins = startBottomLeft;
        barMins.x += barIdx * m_barWidth;

        Vec2 barMaxs = barMins;
        barMaxs.x += m_barWidth;
        float barHeight = RangeMapFloat(
            barData, minYRange, maxYRange, 0.f, boundsHeight );
        barMaxs.y += barHeight;

        // calculate bar color
        Rgba barColor = m_barColor;
        if( m_dataToColor )
            barColor = m_dataToColor( barData, barIdx, minData, maxData );

        AABB2 barBounds = AABB2( barMins, barMaxs );
        AddQuad2D( barBounds, barColor );
    }

    EndSubMesh();
}

vector<float>& BarGraphBuilder::GetDataRef()
{
    m_maxData = -INFINITY;
    m_minData = INFINITY;
    return m_data;
}

void BarGraphBuilder::SetBarWidthFromCount( float fitThisManyBars )
{
    if( fitThisManyBars == 0.f )
        fitThisManyBars = 1.f;
    SetBarWidth( m_bounds.GetWidth() / fitThisManyBars );
}

float BarGraphBuilder::GetMaxData()
{
    if( std::isinf( m_maxData ) )
    {
        for( auto num : m_data )
        {
            if( num > m_maxData )
                m_maxData = num;
        }
    }
    return m_maxData;
}

float BarGraphBuilder::GetMinData()
{
    if( std::isinf( m_minData ) )
    {
        for( auto num : m_data )
        {
            if( num < m_minData )
                m_minData = num;
        }
    }
    return m_minData;
}

void BarGraphBuilder::SetYAxisRange( float minY, float maxY, bool scaleToMaxData /*= true*/, bool scaleToMinData /*= false */ )
{
    m_minYRange = minY;
    m_maxYRange = maxY;
    m_scaleToMaxData = scaleToMaxData;
    m_scaleToMinData = scaleToMinData;
}
