#pragma once
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Core/SmartEnum.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Types.hpp"
#include <string>


class BitmapFont;

class TextMeshBuilder : public MeshBuilder
{
public:
    TextMeshBuilder();
    ~TextMeshBuilder() {};
    void Finalize();
public:

    static void SetDefaultFont( const BitmapFont* font );

    string m_text;


    float m_fontHeight = 10.f;
    float m_aspectScale = 1.f;
    const BitmapFont* m_font = nullptr;
    AABB2 m_boundingBox = AABB2::NEG_ONES_ONES;
    Rgba m_color = Rgba::WHITE;
    TextDrawMode m_textDrawMode = TextDrawMode::OVERRUN;
    Vec2 m_alignment = Vec2::POINT_FIVES; //POINT_FIVES is "Center aligned"

    // Generated data, valid after finalize is called
    Strings m_linesOfText;
    float m_textBlockHeight = 0.f;
    uint m_glyphCount = 0;

private:
    void ReserveVertsAndIndices();
    // Assumes top left alignment, position is the top left of the line
    void AppendLineMeshToVerts( const string& text, const Vec2& position);
    // Assumes bottom left alignment, position is the bottom left of the glyph
    void AppendGlyphMeshToVerts( char glyph, const AABB2& bounds );
    bool HasValidFont();
    void ParseToLinesOfText(); //parses text to strings with '\n'
    void CalculateTextBlockHeight();
    void ShrinkFontToFitInBounds();
    // Wraps lines of text if over the width of the bounding box
    void WrapLines();
    void CalculatePaddingsAndAppendLines();
};
