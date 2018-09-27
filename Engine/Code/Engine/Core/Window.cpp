#include "Engine/Core/WindowsCommon.hpp"
#include <map>

#include "Engine/Core/Window.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Math/AABB2.hpp"

namespace
{
map<HWND, Window*> s_windowFromHWND;
}

// I believe in App you will have a windows procedure you use that looks similar to the following;
// This will be moved to Windows.cpp (here), with a slight tweak;
LRESULT CALLBACK GameWndProc( HWND hwnd,
                              UINT msg,
                              WPARAM wparam,
                              LPARAM lparam )
{
    // NEW:  Give the custom handlers a chance to run first;
    Window *window = s_windowFromHWND[hwnd];
    bool wasConsumend = false;
    if( nullptr != window )
    {
        for( int i = 0; i < window->listeners.size(); ++i )
        {
            bool consumedMsg = window->listeners[i]( msg, wparam, lparam );
            if( consumedMsg )
                wasConsumend = true;
        }
    }

    if( wasConsumend )
        return 0;

    // do default windows behavior (return before this if you don't want default windows behavior for certain messages)
    return ::DefWindowProc( hwnd, msg, wparam, lparam );
}

Window::Window( float clientAspect, char const *title )
{

    m_aspect = clientAspect;

    // Define a window style/class
    WNDCLASSEX windowClassDescription;
    memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
    windowClassDescription.cbSize = sizeof( windowClassDescription );
    windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
    windowClassDescription.lpfnWndProc = GameWndProc; // Register our Windows message-handling function
    windowClassDescription.hInstance = GetModuleHandle( NULL );
    windowClassDescription.hIcon = NULL;
    windowClassDescription.hCursor = NULL;
    windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
    RegisterClassEx( &windowClassDescription );

    // #SD1ToDo: Add support for full screen mode (requires different window style flags than windowed mode)
    const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
    const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

    // Get desktop rect, dimensions, aspect
    RECT desktopRect;
    HWND desktopWindowHandle = GetDesktopWindow();
    GetClientRect( desktopWindowHandle, &desktopRect );
    float desktopWidth = (float) ( desktopRect.right - desktopRect.left );
    float desktopHeight = (float) ( desktopRect.bottom - desktopRect.top );
    float desktopAspect = desktopWidth / desktopHeight;

    // Calculate maximum client size (as some % of desktop size)
    constexpr float maxClientFractionOfDesktop = 0.90f;
    float clientWidth = desktopWidth * maxClientFractionOfDesktop;
    float clientHeight = desktopHeight * maxClientFractionOfDesktop;
    if( clientAspect > desktopAspect )
    {
        // Client window has a wider aspect than desktop; shrink client height to match its width
        clientHeight = clientWidth / clientAspect;
    }
    else
    {
        // Client window has a taller aspect than desktop; shrink client width to match its height
        clientWidth = clientHeight * clientAspect;
    }

    m_windowDimensions.x =  clientWidth;
    m_windowDimensions.y =  clientHeight;

    // Calculate client rect bounds by centering the client area
    float clientMarginX = 0.5f * ( desktopWidth - clientWidth );
    float clientMarginY = 0.5f * ( desktopHeight - clientHeight );
    RECT clientRect;
    clientRect.left = (int) clientMarginX;
    clientRect.right = clientRect.left + (int) clientWidth;
    clientRect.top = (int) clientMarginY;
    clientRect.bottom = clientRect.top + (int) clientHeight;


    // Calculate the outer dimensions of the physical window, including frame et.al.
    RECT windowRect = clientRect;
    AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

    WCHAR windowTitle[1024];
    MultiByteToWideChar( GetACP(), 0, title, -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[0] ) );
    m_hwnd = CreateWindowEx(
        windowStyleExFlags,
        windowClassDescription.lpszClassName,
        windowTitle,
        windowStyleFlags,
        windowRect.left,
        windowRect.top,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL,
        NULL,
        GetModuleHandle( NULL ),
        NULL );
    s_windowFromHWND[(HWND) m_hwnd] = this;
    ShowWindow( (HWND) m_hwnd, SW_SHOW );
    SetForegroundWindow( (HWND) m_hwnd );
    SetFocus( (HWND) m_hwnd );

    HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
    SetCursor( cursor );
}

Window::~Window()
{

}

void Window::RegisterHandler( WindowsMessageHandlerCallback cb )
{
    listeners.push_back( cb );
}

void Window::UnregisterHandler( WindowsMessageHandlerCallback cb )
{
    ContainerUtils::EraseValues( listeners, cb );
}

AABB2 Window::GetWindowBounds() const
{
    return AABB2::MakeBoundsFromDimensions( m_windowDimensions );
}

AABB2 Window::GetWindowBoundsInScreen() const
{
    RECT rect;
    ::GetClientRect( (HWND) m_hwnd, &rect );

    POINT offset;
    offset.x = 0;
    offset.y = 0;
    ::ClientToScreen( (HWND) m_hwnd, &offset );

    rect.left += offset.x;
    rect.right += offset.x;
    rect.top += offset.y;
    rect.bottom += offset.y;

    return AABB2(
        (float) rect.left, (float) rect.bottom,
        (float) rect.right, (float) rect.top );
}

Vec2 Window::GetWindowCenterInScreen()
{
    return WindowToScreen( m_windowDimensions / 2.f );
}

Vec2 Window::GetWindowCenterLocal()
{
    return m_windowDimensions / 2.f;
}

Vec2 Window::WindowToScreen( const Vec2& pos )
{
    POINT center;
    center.x = (LONG) pos.x;
    center.y = (LONG) pos.y;
    ClientToScreen( (HWND) m_hwnd, &center );
    return Vec2( (float) center.x, (float) center.y );
}

Vec2 Window::ScreenToWindow( const Vec2& pos )
{
    POINT center;
    center.x = (LONG) pos.x;
    center.y = (LONG) pos.y;
    ScreenToClient( (HWND) m_hwnd, &center );
    return Vec2( (float) center.x, (float) center.y );
}
