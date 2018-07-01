#pragma once
#include "Engine/Math/IVec2.hpp"
#include "Engine/Core/Types.hpp"
#include "Engine/Renderer/RendererEnums.hpp"

SMART_ENUM(
    TextureFormat,

    RGBA8,
    D24S8
)

class Image;

//---------------------------------------------------------------------------
class Texture
{
    friend class Renderer; // Textures are managed by a Renderer instance
public:
    static Texture* GetWhiteTexture();
    static Texture* GetFlatNormalTexture();
    static Texture* GetBlackTexture();
    static Texture* GetParticleTexture();

    uint GetHandle() const { return m_handle; };
    IVec2 GetDimensions() const { return m_dimensions; };
    float GetAspect() const { return  (float) m_dimensions.x / (float) m_dimensions.y; };
    Image* MakeImageFromGPU();
    Texture() {};
    virtual ~Texture();
    Texture( const String& imageFilePath, bool useMipmaps = true ); // Use renderer->CreateOrGetTexture() instead!
    Texture( Image* image, bool useMipmaps = true );
    bool Valid() const { return m_handle != NULL; };
protected:
    static Texture* CreateCompatible( const Texture* textureSource );
    static void SwapHandle( Texture* textureA, Texture* textureB );
    void MakeFromData( unsigned char* imageData, const IVec2& texelSize, int numComponents );
    virtual void MakeFromImage( Image* image );
    bool CreateRenderTarget( uint width, uint height, TextureFormat format );

    bool m_useMipmaps = true;
    uint			m_handle = 0;
    IVec2		m_dimensions = IVec2::ZEROS;
    TextureFormat   m_format = TextureFormat::RGBA8;
    TextureTarget m_target = TextureTarget::TEXTURE_2D;
};
