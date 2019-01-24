#pragma once

#include <vector>
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Math/Vec2.hpp"

typedef bool( *WindowsMessageHandlerCallback )( unsigned int msg, size_t wparam, size_t lparam );

class AABB2;

class Window
{
public:
    // Do all the window creation work is currently in App
    Window( float clientAspect, char const *title = "Untitled" );
    ~Window();

    // Register a function callback to the Window.  Any time Windows processing a
    // message, this callback should forwarded the information, along with the
    // supplied user argument.
    void RegisterHandler( WindowsMessageHandlerCallback cb );

    // Allow users to unregister (not needed for this Assignment, but I
    // like having cleanup methods).
    void UnregisterHandler( WindowsMessageHandlerCallback cb );

    // This is safely castable to an HWND
    void* GetHandle() const { return m_hwnd; }

    // ** EXTRAS ** //
    // void SetTitle( char const *new_title );

    // When the WindowsProcedure is called - let all listeners get first crack at the message
    // Giving us better code modularity.
    vector<WindowsMessageHandlerCallback> listeners;

    Vec2 GetDimensions() const { return m_windowDimensions; };
    AABB2 GetWindowBounds() const;
    AABB2 GetWindowBoundsInScreen() const;
    float GetAspect() const { return m_aspect; };
    Vec2 GetWindowCenterInScreen();
    Vec2 GetWindowCenterLocal();
    Vec2 WindowToScreen( const Vec2& pos );
    Vec2 ScreenToWindow( const Vec2& pos );
    void* GetWindowHWND() { return m_hwnd; };
    void Tile4by4( int pos ); // [0,3] 0 for top left, 1 for top right

private:

    void* m_hwnd; // intptr_t
    Vec2 m_windowDimensions;
    float m_aspect = 1.f;

};