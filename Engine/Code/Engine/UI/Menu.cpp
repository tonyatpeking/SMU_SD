#include "Engine/UI/Menu.hpp"
#include "Engine/UI/MenuEntry.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/Vec2.hpp"


Menu::Menu( Renderer* renderer, InputSystem* input )
    : m_renderer( renderer )
    , m_input( input )
{
    m_font = m_renderer->DefaultFont();
}

Menu::~Menu()
{
    ContainerUtils::DeletePointers( m_menuEntries );
}

void Menu::Update( float deltaSeconds )
{
    for( auto& entry : m_menuEntries )
        entry->Update( deltaSeconds );

    RefreshEntryBounds();
}

void Menu::Render() const
{
    for( auto& entry : m_menuEntries )
        entry->Render();
}

bool Menu::ProcessMouseInput()
{
    bool overAny = false;
    for( auto& entry : m_menuEntries )
    {
        if( entry->ProcessMouseInput() )
            overAny = true;
    }
    return overAny;
}

void Menu::ExpandSelectedSubMenu()
{
    if( m_selectedEntry )
        m_selectedEntry->ExpandSubMenu();
}

void Menu::CollapseSubMenu()
{
    if( m_selectedEntry )
        m_selectedEntry->CollapseSubMenu();
}

void Menu::SelectNextEntry()
{
    int nextIdx = m_selectedEntryIdx + 1;
    SelectEntry( nextIdx );
}

void Menu::SelectPrevEntry()
{
    int prevIdx = m_selectedEntryIdx - 1;
    SelectEntry( prevIdx );
}

void Menu::RunSelectedEntry()
{
    if( m_selectedEntry )
        m_selectedEntry->Run();
}

void Menu::RefreshEntryBounds()
{
    Vec2 firstPos = m_bounds.RangeMap01ToBounds( Vec2( 0.5f, 1.f ) );
    firstPos.y = firstPos.y - ( m_entryHeight / 2.f ) - m_entrySpacing;
    AABB2 currentBounds = AABB2( firstPos, m_entryWidth, m_entryHeight );
    for( unsigned int entryIdx = 0; entryIdx < m_menuEntries.size(); ++entryIdx )
    {
        m_menuEntries[entryIdx]->SetBounds( currentBounds );
        currentBounds.Translate( Vec2( 0.f, -( m_entryHeight + m_entrySpacing ) ) );
    }
}

void Menu::SelectEntry( int entryIdx )
{
    if( m_selectedEntry )
        m_selectedEntry->Deselect();

    if( m_menuEntries.empty() )
    {
        m_selectedEntry = nullptr;
        return;
    }

    if( entryIdx > (int) m_menuEntries.size() - 1 )
        entryIdx = 0;
    if( entryIdx < 0 )
        entryIdx = (int) m_menuEntries.size() - 1;

    m_selectedEntryIdx = entryIdx;

    m_selectedEntry = m_menuEntries[m_selectedEntryIdx];
    m_selectedEntry->Select();
}

void Menu::AddEntry( MenuEntry* menuEntry )
{
    m_menuEntries.push_back( menuEntry );
    menuEntry->SetParent( this );
}

void Menu::AddEntry( const String& text, std::function<void()> action, const Rgba& color )
{
    MenuEntry* entry = new MenuEntry();
    AddEntry( entry );
    entry->SetText( text );
    entry->SetAction( action );
    entry->SetColor( color );
}

void Menu::RemoveEntry( MenuEntry* menuEntry )
{
    delete menuEntry;
    ContainerUtils::EraseOneValue( m_menuEntries, menuEntry );
    if( m_selectedEntry == menuEntry )
        m_selectedEntry = nullptr;

    if( m_menuEntries.empty() )
        return;

    if( m_selectedEntryIdx > (int) m_menuEntries.size() - 1 )
        m_selectedEntryIdx = (int) m_menuEntries.size() - 1;
    SelectEntry( m_selectedEntryIdx );
}
