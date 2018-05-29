#include "Engine/UI/Menu.hpp"
#include "Engine/UI/MenuEntry.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Input/InputSystem.hpp"

MenuEntry::~MenuEntry()
{
    if( m_subMenu )
        delete m_subMenu;
}

void MenuEntry::Render() const
{
    if( m_isSelected )
    {
        AABB2 largerBounds = m_bounds;
        largerBounds.ScaleWidthFromCenter( 1.1f );
        largerBounds.ScaleHeightFromCenter( 1.3f );
        GetRenderer()->DrawAABB( largerBounds, Rgba::BROWN );

        if( m_subMenu )
            m_subMenu->Render();
    }
    GetRenderer()->DrawTextInBox2D( GetText(), m_bounds, GetHeight(), GetFont(), TextDrawMode::OVERRUN, Vec2(0.f,0.5f), m_color );
}

void MenuEntry::Update( float deltaSeconds )
{
    if( m_subMenu )
        m_subMenu->Update( deltaSeconds );
}

bool MenuEntry::ProcessMouseInput()
{
    bool mouseOver = false;
    InputSystem* input = m_parentMenu->m_input;
    Vec2 mousePos = (Vec2) input->GetCursorWindowPosFlipY();

    if( m_bounds.IsPointInside( mousePos ) )
    {
        Select();
        mouseOver = true;
    }
    else
    {
        Deselect();
    }

    if( m_isSelected && input->WasKeyJustPressed(InputSystem::MOUSE_LEFT) )
    {
        Run();
    }
    return mouseOver;
}

String MenuEntry::GetText() const
{
    return m_text;
}

void MenuEntry::SetBounds( const AABB2& bounds )
{
    m_bounds = bounds;
    m_bounds.SetWidth( GetFont()->GetTextWidth( GetText(), GetHeight() ) );
}

bool MenuEntry::ExpandSubMenu()
{
    if( m_subMenu && !m_isSubMenuExpanded )
        m_isSubMenuExpanded = true;
    return false;
}

bool MenuEntry::CollapseSubMenu()
{
    if( m_subMenu && m_isSubMenuExpanded )
        m_isSubMenuExpanded = false;
    return false;
}

Renderer* MenuEntry::GetRenderer() const
{
    return  m_parentMenu->m_renderer;
}

BitmapFont* MenuEntry::GetFont() const
{
    return m_parentMenu->m_font;
}

float MenuEntry::GetHeight() const
{
    return m_parentMenu->m_entryHeight;
}

void MenuEntry::Run()
{
    m_action();
}

