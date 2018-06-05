#pragma once

#include <vector>

#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba.hpp"

class Image
{
    friend class Texture;
public:
    explicit Image( const String& imageFilePath );
    explicit Image( int x, int y, const Rgba& color = Rgba::BLACK );
    ~Image() {};
    Rgba	GetTexel( int x, int y ) const; 			// (0,0) is top-left
    void	SetTexel( int x, int y, const Rgba& color );
    Rgba    GetTexelAtUV( const Vec2& uv );
    void    SetAll( const Rgba& color );
    IVec2 GetDimentions() const { return m_dimensions; };
    void FlipYCoords();
    bool IsImageValid();
    void SaveToDisk( const String& imageFilePath );

    std::vector<Rgba>	m_texels;
private:
    void CalcInverseDimensions();
    Vec2 m_inversDimensions;
    Vec2 m_halfTexelUV;
    IVec2		m_dimensions;
    // by default ordered left-to-right, then down, from [0]=(0,0) at top-left

    void FillTexelsFromArray( const unsigned char* imageArray, const IVec2& dimensions,
                              int numComponents );
};