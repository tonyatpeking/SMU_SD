// #define WIN32_LEAN_AND_MEAN
// #include <Windows.h>
//#include <gl/GL.h>

// #pragma warning(push, 0)        //suppress warnings
// #include "ThirdParty/stb/stb_image.h"
// #pragma warning(pop)

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/GLFunctions.hpp"
#include "Engine/Core/Rgba.hpp"

namespace
{
int CalculateMipCount( int maxTextureDim )
{
    int dim = maxTextureDim;
    int levels = 0;
    while( dim > 0 )
    {
        dim = dim >> 1;
        ++levels;
    }
    return levels;
}
}

Texture* Texture::GetWhiteTexture()
{
    static Texture* s_whiteTexture = nullptr;
    if( !s_whiteTexture )
    {
        Image tempImage = Image( 1, 1, Rgba::WHITE );
        s_whiteTexture = new Texture( &tempImage );
    }
    return s_whiteTexture;
}

Texture* Texture::GetFlatNormalTexture()
{
    static Texture* s_flatTexture = nullptr;
    if( !s_flatTexture )
    {
        Image tempImage = Image( 1, 1, Rgba( 128, 128, 255, 255 ) );
        s_flatTexture = new Texture( &tempImage );
    }
    return s_flatTexture;
}

Texture* Texture::GetBlackTexture()
{
    static Texture* s_blackTexture = nullptr;
    if( !s_blackTexture )
    {
        Image tempImage = Image( 1, 1, Rgba::BLACK );
        s_blackTexture = new Texture( &tempImage );
    }
    return s_blackTexture;
}

Texture* Texture::GetParticleTexture()
{
    static Texture* s_texture = nullptr;
    if( !s_texture )
    {
        s_texture = new Texture( "Data/Images/particle.png" );
    }
    return s_texture;
}

Texture* Texture::CreateCompatible( const Texture* textureSource )
{
    Texture* texture = new Texture();
    texture->CreateRenderTarget( textureSource->m_dimensions.x, textureSource->m_dimensions.y,
                                 textureSource->m_format );
    return texture;
}

void Texture::SwapHandle( Texture* textureA, Texture* textureB )
{
    uint temp = textureA->GetHandle();
    textureA->m_handle = textureB->GetHandle();
    textureB->m_handle = temp;
}

// Called only by the Renderer.  Use renderer->CreateOrGetTexture() to instantiate textures.
Texture::Texture( const String& imageFilePath, bool useMipmaps /*= true */ )
    : m_useMipmaps( useMipmaps )
{
    Image image = Image( imageFilePath );
    image.FlipYCoords();
    MakeFromImage( &image );
}

Texture::Texture( Image* image, bool useMipmaps )
    : m_useMipmaps( useMipmaps )
{
    MakeFromImage( image );
}


Texture::~Texture()
{
    if( Valid() )
        glDeleteTextures( 1, (GLuint*) &m_handle );
}

// Creates a texture identity on the video card, and populates it with the given image texel data
//
void Texture::MakeFromData( unsigned char* imageData, const IVec2& texelSize, int numComponents )
{

    m_dimensions = texelSize;

    int width = m_dimensions.x;
    int height = m_dimensions.y;

    int mipCount = 1;
    if( m_useMipmaps )
    {
        mipCount = CalculateMipCount( max( width, height ) );
    }

    GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
    if( numComponents == 3 )
        bufferFormat = GL_RGB;

    GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; allows us to translate into a different texture format as we upload to OpenGL


                                          // Tell OpenGL that our pixel data is single-byte aligned
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    // Ask OpenGL for an unused texName (ID number) to use for this texture
    glGenTextures( 1, (GLuint*) &m_handle );

    // Tell OpenGL to bind (set) this as the currently active texture
    glBindTexture( GL_TEXTURE_2D, m_handle );

    glTexStorage2D( GL_TEXTURE_2D, mipCount, GL_RGBA8, width, height );

    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, bufferFormat, GL_UNSIGNED_BYTE, imageData );

    glGenerateMipmap( GL_TEXTURE_2D );  //Generate num_mipmaps number of mipmaps here.


//     glTexImage2D(			// Upload this pixel data to our new OpenGL texture
//                   GL_TEXTURE_2D,		// Creating this as a 2d texture
//                   0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
//                   internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
//                   width,			// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
//                   height,			// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
//                   0,					// Border size, in texels (must be 0 or 1, recommend 0)
//                   bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
//                   GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
//                   imageData );		// Address of the actual pixel data bytes/buffer in system memory

    GL_CHECK_ERROR();

}

void Texture::MakeFromImage( Image* image )
{
    if( !image )
    {
        LOG_NULL_POINTER( "Image" );
        return;
    }
    m_dimensions = image->GetDimentions();
    MakeFromData( reinterpret_cast<unsigned char*>( image->m_texels.data() ), m_dimensions, 4 );
}

bool Texture::CreateRenderTarget( uint width, uint height, TextureFormat format )
{
    // generate the link to this texture
    glGenTextures( 1, &m_handle );
    if( m_handle == NULL )
    {
        return false;
    }

    // TODO - add a TextureFormatToGLFormats( GLenum*, GLenum*, GLenum*, eTextureFormat )
    //        when more texture formats are required;
    GLenum internal_format = GL_RGBA8;
    GLenum channels = GL_RGBA;
    GLenum pixel_layout = GL_UNSIGNED_BYTE;
    if( format == TextureFormat::D24S8 )
    {
        internal_format = GL_DEPTH_STENCIL;
        channels = GL_DEPTH_STENCIL;
        pixel_layout =  GL_UNSIGNED_INT_24_8; // #WHAT?? originally GL_UNSIGNED_INT_24_8
    }

    // Copy the texture - first, get use to be using texture unit 0 for this;
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_handle );    // bind our texture to our current texture unit (0)

    // Copy data into it;
    glTexImage2D( GL_TEXTURE_2D, 0,
                  internal_format, // what's the format OpenGL should use
                  width,
                  height,
                  0,             // border, use 0
                  channels,      // how many channels are there?
                  pixel_layout,  // how is the data laid out
                  nullptr );     // don't need to pass it initialization data

                                 // make sure it succeeded
    GL_CHECK_ERROR();
    // cleanup after myself;
    glBindTexture( GL_TEXTURE_2D, NULL ); // unset it;

                                          // Save this all off
    m_dimensions.x = width;
    m_dimensions.y = height;

    m_format = format; // I save the format with the texture
                    // for sanity checking.

                    // great, success
    return true;
}



Image* Texture::MakeImageFromGPU()
{
    Image* image = new Image( m_dimensions.x, m_dimensions.y );

    glBindTexture( GL_TEXTURE_2D, m_handle );
    glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<unsigned char*>( image->m_texels.data() ) );

    GL_CHECK_ERROR();

    glBindTexture( GL_TEXTURE_2D, NULL );
    return image;
}

