#pragma once

#include <vector>
#include <string>
#include <functional>

#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba.hpp"

class Menu;
class Renderer;
class BitmapFont;

class MenuEntry
{
public:
    virtual ~MenuEntry();

    virtual void Render() const;
    virtual void Update( float deltaSeconds );
    virtual bool ProcessMouseInput();

    void Select() { m_isSelected = true; };
    void Deselect() { m_isSelected = false; };
    bool IsSelected() { return m_isSelected; };

    void SetAction( std::function<void()> action ) { m_action = action; };
    void SetText( const String& text ) { m_text = text; };
    void SetColor( const Rgba& color ) { m_color = color; };
    virtual String GetText() const;
    void SetBounds( const AABB2& bounds );
    void SetParent( Menu* parent ) { m_parentMenu = parent; };
    bool ExpandSubMenu(); // returns true if sub menu was expanded
    bool CollapseSubMenu(); // returns true if sub menu was collapsed

    Renderer* GetRenderer() const;
    BitmapFont* GetFont() const;
    float GetHeight() const;

    virtual void Run();

protected:

    AABB2 m_bounds = AABB2::ZEROS_ONES;
    Rgba m_color;

    bool m_isSubMenuExpanded = false;
    bool m_isSelected = false;
    String m_text;

    // Owns:
    Menu* m_subMenu = nullptr;
    std::function<void()> m_action;
    // Uses:
    Menu* m_parentMenu = nullptr;
};