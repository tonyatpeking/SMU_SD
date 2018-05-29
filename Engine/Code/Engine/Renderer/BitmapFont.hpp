#pragma once
#include <vector>
#include <string>

#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

class BitmapFont : public SpriteSheet
{
    friend class Renderer;
private:
    BitmapFont( const Texture* texture, float bastAspect = 1.f, const IVec2& layout = IVec2( 16, 16 ) );
public:
    AABB2 GetGlyphUVs( int glyphID ) const;
    float GetGlyphAspect( int glyphUnicode ) const;
    float GetTextWidth( const String& text, float height, float glyphAspect = 1.f ) const;

    //#Refactor Move this to StringUtils
    void WrapLineOfText( const String& line, float height, float maxWidth, Strings& out_wrappedLines, float glyphAspect = 1.f ) const;

private:
    float m_baseAspect;
};

