#include "Engine/Renderer/CubeMap.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Image/Image.hpp"
#include "Engine/Renderer/GLFunctions.hpp"
#include "Engine/Core/Rgba.hpp"


CubeMap::CubeMap( const String& imageFilePath )
{
    m_target = TextureTarget::CUBE_MAP;
    Image image = Image( imageFilePath );
    // Don't flip Y because cube maps are backwards
    //image.FlipYCoords();
    MakeFromImage( &image );
}

CubeMap::CubeMap( Image* image )
{
    m_target = TextureTarget::CUBE_MAP;
    MakeFromImage( image );
}

CubeMap::~CubeMap()
{
    if( Valid() )
        glDeleteTextures( 1, (GLuint*) &m_handle );
}

static void BindImageToSide( TextureTarget side, Image* image, uint size,
                             uint ox, uint oy, GLenum channels, GLenum pixel_layout )
{
    uint width = image->GetDimentions().x;
    uint offset = width * oy + ox;
    void const *ptr = reinterpret_cast<unsigned char*>( image->m_texels.data() + offset );
    glTexSubImage2D( ToGLEnum( side ),
                     0,          // mip_level
                     0, 0,       // offset
                     size, size,
                     channels,
                     pixel_layout,
                     ptr );

    GL_CHECK_ERROR();
}

void CubeMap::MakeFromImage( Image* image )
{
    uint width = image->GetDimentions().x;
    uint dim = width / 4;
    m_dimensions =  IVec2( dim, dim );

    if( m_handle == NULL )
    {
        glGenTextures( 1, &m_handle );
    }

    glBindTexture( GL_TEXTURE_CUBE_MAP, m_handle );
    glTexStorage2D( GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, dim, dim );
    GL_CHECK_ERROR();

    glPixelStorei( GL_UNPACK_ROW_LENGTH, width );

    GLenum channels = GL_RGBA;
    GLenum pixel_layout = GL_UNSIGNED_BYTE;

    BindImageToSide( TextureTarget::CUBE_MAP_POSITIVE_X, image, dim, dim * 2, dim * 1, channels, pixel_layout );
    BindImageToSide( TextureTarget::CUBE_MAP_NEGATIVE_X, image, dim, dim * 0, dim * 1, channels, pixel_layout );
    BindImageToSide( TextureTarget::CUBE_MAP_POSITIVE_Y, image, dim, dim * 1, dim * 0, channels, pixel_layout );
    BindImageToSide( TextureTarget::CUBE_MAP_NEGATIVE_Y, image, dim, dim * 1, dim * 2, channels, pixel_layout );
    BindImageToSide( TextureTarget::CUBE_MAP_POSITIVE_Z, image, dim, dim * 1, dim * 1, channels, pixel_layout );
    BindImageToSide( TextureTarget::CUBE_MAP_NEGATIVE_Z, image, dim, dim * 3, dim * 1, channels, pixel_layout );

    glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );

}
