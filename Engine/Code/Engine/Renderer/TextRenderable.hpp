#pragma once
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/RendererEnums.hpp"

class BitmapFont;

class TextRenderable : public Renderable
{
public:
	TextRenderable( const string& str );
	virtual ~TextRenderable();

    void SetText( const string& str );
    string GetText() const;

    // will call finalize if text has changed
    void UpdateText( const string& str );

    void Finalize();

public:
    float m_fontHeight = 10.f;
    float m_aspectScale = 1.f;
    const BitmapFont* m_font = nullptr;
    AABB2 m_boundingBox = AABB2::NEG_ONES_ONES;
    Rgba m_color = Rgba::WHITE;
    TextDrawMode m_textDrawMode = TextDrawMode::OVERRUN;
    Vec2 m_alignment = Vec2::POINT_FIVES; //POINT_FIVES is "Center aligned", (0,1) is top left

    // Generated data
    Strings m_linesOfText;
    float m_textBlockHeight = 0.f;
    uint m_glyphCount = 0;

private:
    string m_text;
};
