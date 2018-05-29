#include <algorithm>

#pragma warning(push, 0)        //suppress warnings
#include "ThirdParty/stb/stb_image.h"
#pragma warning(pop)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include "ThirdParty/stb/stb_image_write.h"

#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorUtils.hpp"


Image::Image( const String& imageFilePath )
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
}

Rgba Image::GetTexel( int x, int y ) const
{
    int texelIdx = x + ( y * m_dimensions.x );
    return m_texels[texelIdx];
}

void Image::SetTexel( int x, int y, const Rgba& color )
{
    int texelIdx = x + ( y * m_dimensions.x );
    m_texels[texelIdx] = color;
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

void Image::SaveToDisk( const String& imageFilePath )
{
    stbi_write_png( imageFilePath.c_str(), m_dimensions.x, m_dimensions.y, 4,
                    m_texels.data(), sizeof( Rgba ) * m_dimensions.x );
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
