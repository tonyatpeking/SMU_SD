#include "Engine/Renderer/TextRenderable.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/TextMeshBuilder.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

TextRenderable::TextRenderable( const String& str )
{
    SetText( str );
}

TextRenderable::~TextRenderable()
{
    DeleteMesh();
}

void TextRenderable::SetText( const String& str )
{
    m_text = str;
}

String TextRenderable::GetText() const
{
    return m_text;
}

void TextRenderable::UpdateText( const String& str )
{
    if( str != m_text )
    {
        SetText( str );
        Finalize();
    }
}

void TextRenderable::Finalize()
{
    GetMaterial( 0 )->SetTint( m_color );
    TextMeshBuilder tmb{};
    tmb.m_fontHeight = m_fontHeight;
    tmb.m_aspectScale = m_aspectScale;
    tmb.m_font = m_font;
    tmb.m_boundingBox = m_boundingBox;
    tmb.m_color = m_color;
    tmb.m_textDrawMode = m_textDrawMode;
    tmb.m_alignment = m_alignment;
    tmb.m_text = m_text;
    tmb.Finalize();

    DeleteMesh();

    SetMesh( tmb.MakeMesh() );

    GetMaterial( 0 )->m_diffuse = tmb.m_font->GetTexture();
    GetMaterial( 0 )->m_shaderPass = ShaderPass::GetDefaultUIShader();

    m_linesOfText = tmb.m_linesOfText;
    m_textBlockHeight = tmb.m_textBlockHeight;
    m_glyphCount = tmb.m_glyphCount;
}
