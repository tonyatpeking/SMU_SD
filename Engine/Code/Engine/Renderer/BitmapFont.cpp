#include <sstream>

#include "BitmapFont.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/String/StringUtils.hpp"


BitmapFont::BitmapFont( const Texture* texture, float bastAspect, const IVec2& layout )
    : SpriteSheet( texture, layout )
    , m_baseAspect( bastAspect )
{

}

AABB2 BitmapFont::GetGlyphUVs( int glyphID ) const
{
    IVec2 layout = GetLayout();
    IVec2 coords = IVec2::IndexToCoords( glyphID, layout.x );
    coords.y = layout.y - 1 - coords.y;
    AABB2 uvs = GetUVsForSpriteCoords( coords );
    // shrink just a little so that the sampler never samples from above/below rows
    uvs.ScaleHeightFromCenter( 0.98f );
    return uvs;
}

float BitmapFont::GetGlyphAspect( int glyphUnicode ) const
{
    UNUSED( glyphUnicode );
    return m_baseAspect;
}

float BitmapFont::GetTextWidth( const string& text, float height, float glyphAspect ) const
{
    return ( (float) text.length() ) * height * glyphAspect;
}

void BitmapFont::WrapLineOfText( const string& line, float height, float maxWidth, Strings& out_wrappedLines, float glyphAspect /*= 1.f */ ) const
{
    out_wrappedLines.clear();
    if( GetTextWidth( line, height, glyphAspect ) <= maxWidth )
    {
        out_wrappedLines.push_back( line );
        return;
    }
    Strings tokens;
    StringUtils::ParseToTokens( line, tokens, " ", false );
    std::ostringstream oss;
    float currentLineLength = 0.f;
    float spaceWidth = GetTextWidth( " ", height, glyphAspect );
    for( unsigned int tokenIdx = 0; tokenIdx < tokens.size(); ++tokenIdx )
    {
        const string& token = tokens[tokenIdx];
        float tokenWidth = GetTextWidth( token, height, glyphAspect );
        float prevLineLenth = currentLineLength;
        currentLineLength += tokenWidth;
        if( prevLineLenth != 0.f )
            currentLineLength += spaceWidth;
        if( currentLineLength <= maxWidth || prevLineLenth == 0.f )
        {
            if( prevLineLenth != 0.f )
                oss << " ";
            oss << token;
        }
        else
        {
            currentLineLength = 0.f;
            out_wrappedLines.push_back( oss.str() );
            oss = std::ostringstream();
            --tokenIdx;
        }
    }

    if( oss.str().size() > 0 )
        out_wrappedLines.push_back( oss.str() );
}

