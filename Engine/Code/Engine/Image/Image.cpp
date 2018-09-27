#include <algorithm>

#pragma warning(push, 0)        //suppress warnings
#include "ThirdParty/stb/stb_image.h"
#pragma warning(pop)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include "ThirdParty/stb/stb_image_write.h"

#include "Engine/Image/Image.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Math/MathUtils.hpp"


Image::Image( const string& imageFilePath )
{
    // Filled in for us to indicate how many color/alpha components the image had
    // (e.g. 3=RGB, 4=RGBA)
    int numComponents = 0;
    // don't care; we support 3 (RGB) or 4 (RGBA)
    int numComponentsRequested = 0;

    // Load (and decompress) the image RGB(A) bytes from a file on disk,
    // and create an OpenGL texture instance from it
    unsigned char* imageData = stbi_load(
        imageFilePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents,
        numComponentsRequested );

    if( imageData == nullptr )
        LOG_ASSET_LOAD_FAILED( imageFilePath );

    FillTexelsFromArray( imageData, m_dimensions, numComponents );
    stbi_image_free( imageData );
    CalcInverseDimensions();
}

Image::Image( int x, int y, const Rgba& color /*= Rgba::BLACK */ )
    : m_dimensions( x, y )
{
    int numTexels = x * y;
    m_texels.clear();
    m_texels.reserve( numTexels );
    for( int texelIdx = 0; texelIdx < numTexels; ++texelIdx )
    {
        m_texels.emplace_back( color );
    }
    CalcInverseDimensions();
}

Rgba Image::GetTexel( int x, int y ) const
{
    x = ClampInt( x, 0, m_dimensions.x - 1 );
    y = ClampInt( y, 0, m_dimensions.y - 1 );
    int texelIdx = x + ( y * m_dimensions.x );
    return m_texels[texelIdx];
}

void Image::SetTexel( int x, int y, const Rgba& color )
{
    int texelIdx = x + ( y * m_dimensions.x );
    m_texels[texelIdx] = color;
}

Rgba Image::GetTexelAtUV( const Vec2& uv )
{
    // indices of surrounding pixels
    // 2 3
    // 0 1
    float u = uv.u;
    float v = uv.v;
    int x0 = FloorToInt( (uv.x - m_halfTexelUV.x) *  m_dimensions.x );
    int y0 = FloorToInt( (uv.y - m_halfTexelUV.y) * m_dimensions.y );

    float u0 = (float) x0 *  m_inversDimensions.x + m_halfTexelUV.x;
    float v0 = (float) y0 *  m_inversDimensions.y + m_halfTexelUV.y;

    float u3 = u0 + m_inversDimensions.x;
    float v3 = v0 + m_inversDimensions.y;

    float fractionRight = GetFractionInRange( u, u0, u3 );
    fractionRight = Clampf01( fractionRight );
    float fractionTop = GetFractionInRange( v, v0, v3 );
    fractionTop = Clampf01( fractionTop );
    float fractionLeft = 1 - fractionRight;
    float fractionBottom = 1 - fractionTop;

    Rgba texel0 = GetTexel( x0, y0 );
    Rgba texel1 = GetTexel( x0 + 1, y0 );
    Rgba texel2 = GetTexel( x0, y0 + 1 );
    Rgba texel3 = GetTexel( x0 + 1, y0 + 1 );

    return texel0 * ( fractionLeft * fractionBottom )
        + texel1 * ( fractionRight * fractionBottom )
        + texel2 * ( fractionLeft * fractionTop )
        + texel3 * ( fractionRight * fractionTop );
}

void Image::SetAll( const Rgba& color )
{
    for( auto& texel : m_texels )
        texel = color;
}

void Image::FlipYCoords()
{
    if( !IsImageValid() )
    {
        LOG_WARNING( "failed to FlipYCoords, invalid image" );
        return;
    }

    size_t rowSize = sizeof( Rgba ) * m_dimensions.x;
    void* rowBuffer = malloc( rowSize );
    int endRowIdx = m_dimensions.y / 2; // yes we want int division

    // #OPTIMIZE Instead of swapping rows, Word swapping will probably be faster
    // due to hotter caching
    // https://stackoverflow.com/questions/8166502/c-fastest-method-to-swap-two-memory-blocks-of-equal-size
    for( int rowIdx = 0; rowIdx < endRowIdx; ++rowIdx )
    {
        void* rowPtr = &( m_texels[rowIdx * m_dimensions.x] );
        void* complementRowPtr
            = &( m_texels[( m_dimensions.y - 1 - rowIdx ) * m_dimensions.x] );
        memcpy( rowBuffer, rowPtr, rowSize );
        memcpy( rowPtr, complementRowPtr, rowSize );
        memcpy( complementRowPtr, rowBuffer, rowSize );
    }
    free( rowBuffer );
}

bool Image::IsImageValid()
{
    if( m_dimensions.x * m_dimensions.y != m_texels.size() )
    {
        return false;
    }
    return true;
}

void Image::SaveToDisk( const string& imageFilePath )
{
    stbi_write_png( imageFilePath.c_str(), m_dimensions.x, m_dimensions.y, 4,
                    m_texels.data(), sizeof( Rgba ) * m_dimensions.x );
}

void Image::CalcInverseDimensions()
{
    m_inversDimensions = Vec2::ONES / Vec2( m_dimensions );
    m_halfTexelUV = m_inversDimensions * 0.5f;
}

void Image::FillTexelsFromArray( const unsigned char* imageArray,
                                 const IVec2& dimensions, int numComponents )
{
    if( numComponents != 3 && numComponents != 4 )
        LOG_WARNING( "Trying to set Image with wrong number of channels!" );

    int numTexels = dimensions.x * dimensions.y;
    //int numChars = numTexels * numComponents;
    m_texels.clear();
    m_texels.reserve( numTexels );
    // TODO: test if memcpy method is actually faster
    if( numComponents == 4 )
    {
        for( int texelIdx = 0; texelIdx < numTexels; ++texelIdx )
        {
            int charIdx = texelIdx * 4;
            m_texels.emplace_back( imageArray[charIdx], imageArray[charIdx + 1],
                                   imageArray[charIdx + 2], imageArray[charIdx + 3] );
        }
    }
    if( numComponents == 3 )
    {
        for( int texelIdx = 0; texelIdx < numTexels; ++texelIdx )
        {
            int charIdx = texelIdx * 3;
            m_texels.emplace_back( imageArray[charIdx], imageArray[charIdx + 1],
                                   imageArray[charIdx + 2], (unsigned char) 255 );
        }
    }
}
