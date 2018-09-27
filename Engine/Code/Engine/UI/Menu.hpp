#pragma once

#include <vector>
#include <functional>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba.hpp"

class Renderer;
class InputSystem;
class MenuEntry;
class BitmapFont;


class Menu
{
    friend class MenuEntry;
public:
    Menu( Renderer* renderer, InputSystem* input );
    virtual ~Menu();
    virtual void Update( float deltaSeconds );
    virtual void Render() const;
    virtual bool ProcessMouseInput(); // returns true if mouse is over entries

    void SetRenderer( Renderer* renderer ) { m_renderer = renderer; };
    void SetInputSystem( InputSystem* input ) { m_input = input; };
    virtual void ExpandSelectedSubMenu();
    virtual void CollapseSubMenu();
    virtual void SelectNextEntry();
    virtual void SelectPrevEntry();
    virtual void RunSelectedEntry();
    virtual void RefreshEntryBounds();
    void SelectEntry( int entryIdx );
    void SetParent( MenuEntry* parent ) { m_parentEntry = parent; };
    int GetSelectedEntryIdx() { return m_selectedEntryIdx; };

    void AddEntry( const string& text, std::function<void()> action, const Rgba& color = Rgba::WHITE );

    void AddEntry( MenuEntry* menuEntry);
    void RemoveEntry( MenuEntry* menuEntry );

    Vec2 m_nextEmptyPosition;
    AABB2 m_bounds = AABB2::ZEROS_ONES;
    float m_entryHeight = 25.f;
    float m_entryWidth = 400.f;
    float m_entrySpacing = 10.f;

    BitmapFont* m_font = nullptr;


protected:
    int m_selectedEntryIdx = 0;
    MenuEntry* m_selectedEntry = nullptr;
    // Owns:
    vector<MenuEntry*> m_menuEntries;
    // Uses:
    MenuEntry* m_activeSubMenu = nullptr;
    Renderer* m_renderer;
    InputSystem* m_input;
    MenuEntry* m_parentEntry = nullptr;
};