#include "Engine/Renderer/TextMeshBuilder.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Material.hpp"
namespace
{
const BitmapFont* s_defaultFont;

}

void TextMeshBuilder::ReserveVertsAndIndices()
{
    uint numGlyphs = (uint) m_text.size(); // the actual size might be smaller since '\n' is not rendered
    Reserve( numGlyphs * 4, numGlyphs * 6 );
}

TextMeshBuilder::TextMeshBuilder()
    : m_font( s_defaultFont )
{

}

void TextMeshBuilder::Finalize()
{
    if( !HasValidFont() )
        return;

    BeginSubMesh();

    ReserveVertsAndIndices();

    ParseToLinesOfText();

    if( TextDrawMode::WORD_WRAP == m_textDrawMode )
        WrapLines();

    CalculateTextBlockHeight();

    if( TextDrawMode::WORD_WRAP == m_textDrawMode || TextDrawMode::SHRINK_FIT == m_textDrawMode )
    {
        ShrinkFontToFitInBounds();
        CalculateTextBlockHeight(); // recalculate, since font height has changed
    }

    CalculatePaddingsAndAppendLines();

    EndSubMesh();
}

void TextMeshBuilder::SetDefaultFont( const BitmapFont* font )
{
    s_defaultFont = font;
}

void TextMeshBuilder::AppendLineMeshToVerts( const String& text, const Vec2& position )
{
    // Set starting position to bottom left corner
    float aspectRatio = m_font->GetGlyphAspect( 0 ) * m_aspectScale;
    Vec2 offset = Vec2( 0.f, -m_fontHeight );
    // startPos Assumes it is at the bottom left of the glyph
    Vec2 startPos = position + offset;

    float glyphWidth = m_fontHeight * aspectRatio;
    AABB2 glyphBounds = AABB2( 0, 0, glyphWidth, m_fontHeight );
    glyphBounds.Translate( startPos );

    for( unsigned int glyphIdx = 0; glyphIdx < text.length(); ++glyphIdx )
    {
        AppendGlyphMeshToVerts( text[glyphIdx], glyphBounds );
        glyphBounds.Translate( glyphWidth, 0 );
    }
}


void TextMeshBuilder::AppendGlyphMeshToVerts( char glyph, const AABB2& bounds )
{
    AABB2 glyphUVs = m_font->GetGlyphUVs( glyph );

    AddQuad2D( bounds, m_color, glyphUVs );
    ++m_glyphCount;
}

bool TextMeshBuilder::HasValidFont()
{
    if( m_font == nullptr )
    {
        m_font = s_defaultFont;
    }
    if( m_font == nullptr )
    {
        LOG_WARNING( "No default font found for TextMeshBuilder, please set with TextMeshBuilder::SetDefaultFont" );
        return false;
    }
    return true;
}

void TextMeshBuilder::ParseToLinesOfText()
{
    m_linesOfText.clear();
    StringUtils::ParseToTokens( m_text, m_linesOfText, "\n", false );
}

void TextMeshBuilder::CalculateTextBlockHeight()
{
    m_textBlockHeight = (float) m_linesOfText.size() * m_fontHeight;
}

void TextMeshBuilder::ShrinkFontToFitInBounds()
{
    float maxWidth = 0.f;
    for( auto& line : m_linesOfText )
    {
        float width = m_font->GetTextWidth( line, m_fontHeight, m_aspectScale );
        if( width > maxWidth )
            maxWidth = width;
    }
    float shrinkWidthRatio = m_boundingBox.GetWidth() / maxWidth;
    float shrinkHeightRatio = m_boundingBox.GetHeight() / m_textBlockHeight;
    float shrinkRatio = Minf( shrinkWidthRatio, shrinkHeightRatio );
    shrinkRatio = Clampf01( shrinkRatio );
    m_fontHeight *= shrinkRatio;
}

void TextMeshBuilder::WrapLines()
{
    Strings allWrappedLines;
    for( unsigned int lineIdx = 0; lineIdx < m_linesOfText.size(); ++lineIdx )
    {
        const String& line = m_linesOfText[lineIdx];
        Strings wrappedLines;
        m_font->WrapLineOfText( line, m_fontHeight, m_boundingBox.GetWidth(),
                                wrappedLines, m_aspectScale );
        allWrappedLines.insert( allWrappedLines.end(), wrappedLines.begin(), wrappedLines.end() );
    }
    m_linesOfText = allWrappedLines;
}

void TextMeshBuilder::CalculatePaddingsAndAppendLines()
{
    Vec2 pos;
    float totalYPadding = m_boundingBox.GetHeight() - m_textBlockHeight;
    float paddingOnTop = ( 1.f - m_alignment.y ) * totalYPadding;
    pos.y = m_boundingBox.maxs.y - paddingOnTop;
    for( unsigned int lineIdx = 0; lineIdx < m_linesOfText.size(); ++lineIdx )
    {
        String& line = m_linesOfText[lineIdx];
        float totalXPaddingForLine = m_boundingBox.GetWidth()
            - m_font->GetTextWidth( line, m_fontHeight, m_aspectScale );
        float paddingOnLeft = m_alignment.x * totalXPaddingForLine;
        pos.x = m_boundingBox.mins.x + paddingOnLeft;
        AppendLineMeshToVerts( line, pos );
        pos.y -= m_fontHeight;
    }
}
