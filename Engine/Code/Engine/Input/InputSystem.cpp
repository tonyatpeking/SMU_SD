#include <algorithm>
#include "Engine/Core/WindowsCommon.hpp"
#include <Xinput.h> // include the Xinput API
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use 9_1_0 explicitly for broadest compatibility
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/InputObserver.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Math/IVec2.hpp"

namespace
{
InputSystem* g_defaultInputSystem = nullptr;
}

const unsigned char	InputSystem::MOUSE_LEFT             = VK_LBUTTON;
const unsigned char	InputSystem::MOUSE_RIGHT            = VK_RBUTTON;
const unsigned char	InputSystem::MOUSE_MIDDLE           = VK_MBUTTON;

const unsigned char	InputSystem::KEYBOARD_ESCAPE        = VK_ESCAPE;
const unsigned char InputSystem::KEYBOARD_F1            = VK_F1;
const unsigned char InputSystem::KEYBOARD_F2            = VK_F2;
const unsigned char InputSystem::KEYBOARD_F3            = VK_F3;
const unsigned char InputSystem::KEYBOARD_F4            = VK_F4;
const unsigned char InputSystem::KEYBOARD_F5            = VK_F5;
const unsigned char InputSystem::KEYBOARD_F6            = VK_F6;
const unsigned char InputSystem::KEYBOARD_F7            = VK_F7;
const unsigned char InputSystem::KEYBOARD_F8            = VK_F8;
const unsigned char InputSystem::KEYBOARD_F9            = VK_F9;
const unsigned char InputSystem::KEYBOARD_F10           = VK_F10;
const unsigned char InputSystem::KEYBOARD_F11           = VK_F11;
const unsigned char InputSystem::KEYBOARD_F12           = VK_F12;
const unsigned char InputSystem::KEYBOARD_SEMICOLON     = VK_OEM_1;         // ';:'
const unsigned char InputSystem::KEYBOARD_PLUS          = VK_OEM_PLUS;      // '+'
const unsigned char InputSystem::KEYBOARD_COMMA         = VK_OEM_COMMA;     // ','
const unsigned char InputSystem::KEYBOARD_MINUS         = VK_OEM_MINUS;     // '-'
const unsigned char InputSystem::KEYBOARD_PERIOD        = VK_OEM_PERIOD;    // '.'
const unsigned char InputSystem::KEYBOARD_FORWARD_SLASH = VK_OEM_2;         // '/?'
const unsigned char InputSystem::KEYBOARD_TILDE         = VK_OEM_3;         // '`~'
const unsigned char InputSystem::KEYBOARD_L_BRACKET     = VK_OEM_4;         // '[{'
const unsigned char InputSystem::KEYBOARD_BACK_SLASH    = VK_OEM_5;         // '\|'
const unsigned char InputSystem::KEYBOARD_R_BRACKET     = VK_OEM_6;         // ']}'
const unsigned char InputSystem::KEYBOARD_QUOTE         = VK_OEM_7;         // ''"'
const unsigned char InputSystem::KEYBOARD_BACKSPACE     = VK_BACK;
const unsigned char InputSystem::KEYBOARD_TAB           = VK_TAB;
const unsigned char InputSystem::KEYBOARD_ENTER         = VK_RETURN;
const unsigned char InputSystem::KEYBOARD_SHIFT         = VK_SHIFT;
const unsigned char InputSystem::KEYBOARD_CONTROL       = VK_CONTROL;
const unsigned char InputSystem::KEYBOARD_ALT           = VK_MENU;
const unsigned char InputSystem::KEYBOARD_PAUSE         = VK_PAUSE;
const unsigned char InputSystem::KEYBOARD_CAPSLOCK      = VK_CAPITAL;
const unsigned char InputSystem::KEYBOARD_L_SHIFT       = VK_LSHIFT;
const unsigned char InputSystem::KEYBOARD_R_SHIFT       = VK_RSHIFT;
const unsigned char InputSystem::KEYBOARD_L_CONTROL     = VK_LCONTROL;
const unsigned char InputSystem::KEYBOARD_R_CONTROL     = VK_RCONTROL;
const unsigned char InputSystem::KEYBOARD_L_ALT         = VK_LMENU;
const unsigned char InputSystem::KEYBOARD_R_ALT         = VK_RMENU;
const unsigned char InputSystem::KEYBOARD_SPACE         = VK_SPACE;
const unsigned char InputSystem::KEYBOARD_PAGEUP        = VK_PRIOR;
const unsigned char InputSystem::KEYBOARD_PAGEDOWN      = VK_NEXT;
const unsigned char InputSystem::KEYBOARD_END           = VK_END;
const unsigned char InputSystem::KEYBOARD_HOME          = VK_HOME;
const unsigned char InputSystem::KEYBOARD_SCREENSHOT    = VK_SNAPSHOT;
const unsigned char InputSystem::KEYBOARD_INSERT        = VK_INSERT;
const unsigned char InputSystem::KEYBOARD_DELETE        = VK_DELETE;
const unsigned char InputSystem::KEYBOARD_NUMPAD0       = VK_NUMPAD0;
const unsigned char InputSystem::KEYBOARD_NUMPAD1       = VK_NUMPAD1;
const unsigned char InputSystem::KEYBOARD_NUMPAD2       = VK_NUMPAD2;
const unsigned char InputSystem::KEYBOARD_NUMPAD3       = VK_NUMPAD3;
const unsigned char InputSystem::KEYBOARD_NUMPAD4       = VK_NUMPAD4;
const unsigned char InputSystem::KEYBOARD_NUMPAD5       = VK_NUMPAD5;
const unsigned char InputSystem::KEYBOARD_NUMPAD6       = VK_NUMPAD6;
const unsigned char InputSystem::KEYBOARD_NUMPAD7       = VK_NUMPAD7;
const unsigned char InputSystem::KEYBOARD_NUMPAD8       = VK_NUMPAD8;
const unsigned char InputSystem::KEYBOARD_NUMPAD9       = VK_NUMPAD9;
const unsigned char InputSystem::KEYBOARD_MULTIPLY      = VK_MULTIPLY;
const unsigned char InputSystem::KEYBOARD_ADD           = VK_ADD;
const unsigned char InputSystem::KEYBOARD_SEPARATOR     = VK_SEPARATOR;
const unsigned char InputSystem::KEYBOARD_SUBTRACT      = VK_SUBTRACT;
const unsigned char InputSystem::KEYBOARD_DECIMAL       = VK_DECIMAL;
const unsigned char InputSystem::KEYBOARD_DIVIDE        = VK_DIVIDE;
const unsigned char InputSystem::KEYBOARD_UP_ARROW      = VK_UP;
const unsigned char InputSystem::KEYBOARD_LEFT_ARROW    = VK_LEFT;
const unsigned char InputSystem::KEYBOARD_DOWN_ARROW    = VK_DOWN;
const unsigned char InputSystem::KEYBOARD_RIGHT_ARROW   = VK_RIGHT;


void RunMessagePump()
{
    MSG queuedMessage;
    for( ;; )
    {
        const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
        if( !wasMessagePresent )
        {
            break;
        }

        TranslateMessage( &queuedMessage );
        DispatchMessage( &queuedMessage );
    }
}

InputSystem::InputSystem( Window* window )
    : m_controllers{
    XboxController( 0 )
    , XboxController( 1 )
    , XboxController( 2 )
    , XboxController( 3 ) }
    , m_window( window )
{
    g_defaultInputSystem = this;
}


InputSystem* InputSystem::GetDefault()
{
    return g_defaultInputSystem;
}

void InputSystem::BeginFrame()
{
    ClearControllerJustChangedFlags(); //don't need this since the controller states are all set anyway
    ClearKeyboardJustChangedFlags();
    RunMessagePump(); // Ask Windows to call our registered WinProc function with WM_KEYDOWN notifications, etc.

    // Xbox Controllers
    for( int controllerID = 0; controllerID < NUM_CONTROLLERS; ++controllerID )
    {
        UpdateController( controllerID );
    }

    // Cursor
    m_cursorCurrWindowPos = GetCursorWindowPos();
}


void InputSystem::EndFrame()
{
    if( m_cursorSettings.lock )
    {
        Vec2 winCenter = m_window->GetWindowCenterLocal();
        SetCursorWindowPos( winCenter );
        m_cursorPrevWindowPos = winCenter;
    }
    else
        m_cursorPrevWindowPos = m_cursorCurrWindowPos;
    m_mouseWheelDelta = 0;
    m_justLostFocus = false;
    m_justGainedFocus = false;

}

void InputSystem::OnKeyPressed( unsigned char keyCode )
{
    m_keyStates[keyCode].m_isDown = true;
    m_keyStates[keyCode].m_justPressed = true;

    for( size_t observerIdx = 0; observerIdx < m_inputObservers.size(); ++observerIdx )
    {
        m_inputObservers[observerIdx]->NotifyKeyPressed( keyCode );
    }
}

void InputSystem::OnKeyReleased( unsigned char keyCode )
{
    m_keyStates[keyCode].m_isDown = false;
    m_keyStates[keyCode].m_justReleased = true;

    for( size_t observerIdx = 0; observerIdx < m_inputObservers.size(); ++observerIdx )
    {
        m_inputObservers[observerIdx]->NotifyKeyReleased( keyCode );
    }
}

void InputSystem::OnCharInput( unsigned char asciiCode )
{
    for( size_t observerIdx = 0; observerIdx < m_inputObservers.size(); ++observerIdx )
    {
        m_inputObservers[observerIdx]->NotifyCharInput( asciiCode );
    }
}

void InputSystem::AccumMouseWheelDelta( int zDelta )
{
    m_mouseWheelDelta += zDelta;
}

bool InputSystem::IsKeyPressed( unsigned char keyCode ) const
{
    return m_keyStates[keyCode].m_isDown;
}

bool InputSystem::WasKeyJustPressed( unsigned char keyCode ) const
{
    return m_keyStates[keyCode].m_justPressed;
}

bool InputSystem::WasKeyJustReleased( unsigned char keyCode ) const
{
    return m_keyStates[keyCode].m_justReleased;
}

bool InputSystem::WasAnyKeyJustPressed() const
{
    for( int keyCode = 0; keyCode < InputSystem::NUM_KEYS; ++keyCode )
    {
        if( WasKeyJustPressed( (unsigned char) keyCode ) )
            return true;
    }
    return false;
}

Vec2 InputSystem::GetCursorScreenPos() const
{
    POINT cursor;
    GetCursorPos( &cursor );
    return Vec2( (float) cursor.x, (float) cursor.y );
}

Vec2 InputSystem::GetCursorWindowPos() const
{
    return m_window->ScreenToWindow( GetCursorScreenPos() );
}

Vec2 InputSystem::GetCursorDelta() const
{
    return m_cursorCurrWindowPos - m_cursorPrevWindowPos;
}

Vec2 InputSystem::GetCursorWindowPosFlipY() const
{
    Vec2 pos = GetCursorWindowPos();
    pos.y = m_window->GetDimensions().y - pos.y;
    return pos;
}

void InputSystem::SetCursorScreenPos( const Vec2& pos )
{
    ::SetCursorPos( (int) pos.x, (int) pos.y );
}

void InputSystem::SetCursorWindowPos( const Vec2& pos )
{
    Vec2 screenPos = m_window->WindowToScreen( pos );
    SetCursorScreenPos( screenPos );
}

void InputSystem::LockCursor( bool lock )
{
    m_cursorSettings.lock = lock;
}

void InputSystem::ShowCursor( bool show )
{
    ::ShowCursor( show );
    m_cursorSettings.show = show;
}

void InputSystem::ClipCursor( bool clip )
{
    m_cursorSettings.clip = clip;
    if( !clip )
    {
        ::ClipCursor( nullptr ); // this unlock the mouse
    }
    else
    {
        HWND hwnd = (HWND) m_window->GetHandle(); // Get your windows HWND

        RECT client_rect; // window class RECT
        ::GetClientRect( hwnd, &client_rect );

        POINT offset;
        offset.x = 0;
        offset.y = 0;
        ::ClientToScreen( hwnd, &offset );

        client_rect.left += offset.x;
        client_rect.right += offset.x;
        client_rect.top += offset.y;
        client_rect.bottom += offset.y;

        ::ClipCursor( &client_rect );
    }
}

void InputSystem::ShowAndUnlockCursor()
{
    LockCursor( false );
    ClipCursor( false );
    ShowCursor( true );
}

void InputSystem::PushCursorSettings()
{
    m_cursorModeStack.push( m_cursorSettings );
}

void InputSystem::PopCursorSettings()
{
    if( m_cursorModeStack.empty() )
    {
        ShowAndUnlockCursor();
        return;
    }
    m_cursorSettings = m_cursorModeStack.top();
    m_cursorModeStack.pop();
    LockCursor( m_cursorSettings.lock );
    ShowCursor( m_cursorSettings.show );
    ClipCursor( m_cursorSettings.clip );
}

void InputSystem::UpdateClipCursorAfterResize()
{
    if( m_cursorSettings.clip )
        ClipCursor( m_cursorSettings.clip );
}

void InputSystem::SetWindowHasFocus( bool focus )
{
    if( !m_hasFocus && focus )
    {
        m_justGainedFocus = true;
        PopCursorSettings();
    }
    else if( m_hasFocus && !focus )
    {
        PushCursorSettings();
        ShowAndUnlockCursor();
        m_justLostFocus = true;
    }
    m_hasFocus = focus;
}

bool InputSystem::DidWindowJustLoseFocus()
{
    return m_justLostFocus;
}

bool InputSystem::DidWindowJustGainFocus()
{
    return m_justGainedFocus;
}

XboxController* InputSystem::GetFirstConnectedController()
{
    for( int controllerID = 0; controllerID < NUM_CONTROLLERS; ++controllerID )
    {
        if( m_controllers[controllerID].IsConnected() )
            return &( m_controllers[controllerID] );
    }
    return nullptr;
}


void InputSystem::Attach( InputObserver* inputObserver )
{
    Detach( inputObserver );
    m_inputObservers.push_back( inputObserver );
}

void InputSystem::Detach( InputObserver* inputObserver )
{
    auto it = std::find( m_inputObservers.begin(), m_inputObservers.end(), inputObserver );

    if( it != m_inputObservers.end() )
        m_inputObservers.erase( it );
}

void InputSystem::ClearKeyboardJustChangedFlags()
{
    // Clear all just-changed flags, in preparation for the next round of WM_KEYDOWN, etc. messages
    for( int keyCode = 0; keyCode < InputSystem::NUM_KEYS; ++keyCode )
    {
        m_keyStates[keyCode].m_justPressed = false;
        m_keyStates[keyCode].m_justReleased = false;
    }
}

void InputSystem::ClearControllerJustChangedFlags()
{
    for( int controllerID = 0; controllerID < NUM_CONTROLLERS; ++controllerID )
    {
        m_controllers[controllerID].ClearJustChangedFlags();
    }
}

void InputSystem::UpdateController( int controllerID )
{
    XINPUT_STATE xboxControllerState;
    memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
    DWORD errorStatus = XInputGetState( controllerID, &xboxControllerState );
    if( errorStatus == ERROR_SUCCESS )
    {
        m_controllers[controllerID].UpdateIsConnected( true );
        // Triggers
        m_controllers[controllerID].UpdateLeftTrigger( xboxControllerState.Gamepad.bLeftTrigger );
        m_controllers[controllerID].UpdateRightTrigger( xboxControllerState.Gamepad.bRightTrigger );
        // Joysticks
        m_controllers[controllerID].GetJoystick( XboxController::LEFT_JOYSTICK )
            .OnChangePosition( xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY );
        m_controllers[controllerID].GetJoystick( XboxController::RIGHT_JOYSTICK )
            .OnChangePosition( xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY );
        // Buttons
        m_controllers[controllerID].UpdateButtons( xboxControllerState.Gamepad.wButtons );

    }
    else
    {
        m_controllers[controllerID].UpdateIsConnected( false );
    }
}


void InputSystem::OnKeyPressedWithRepeat( unsigned char keyCode )
{
    for( size_t observerIdx = 0; observerIdx < m_inputObservers.size(); ++observerIdx )
    {
        m_inputObservers[observerIdx]->NotifyKeyPressedWithRepeat( keyCode );
    }
}
