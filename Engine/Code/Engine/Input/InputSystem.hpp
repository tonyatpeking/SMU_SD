#pragma once
#include <vector>
#include <stack>

#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/Vec2.hpp"

class InputObserver;
class IVec2;
class Window;

class InputSystem
{

public:
    InputSystem( Window* window );
    ~InputSystem() {};

    static InputSystem* GetDefault();

    void BeginFrame();
    void EndFrame();

    // Notification Methods ( notify the input system of a key press )
    void OnKeyPressed( unsigned char keyCode );
    void OnKeyReleased( unsigned char keyCode );
    void OnCharInput( unsigned char asciiCode );
    void AccumMouseWheelDelta( int zDelta ); // goes in increments of 120
    void OnKeyPressedWithRepeat( unsigned char keyCode );


    // Query Methods
    bool IsKeyPressed( unsigned char keyCode ) const;
    bool WasKeyJustPressed( unsigned char keyCode ) const;
    bool WasKeyJustReleased( unsigned char keyCode ) const;
    bool WasAnyKeyJustPressed() const;
    int GetMouseWheelDelta() const { return m_mouseWheelDelta; };
    Vec2 GetCursorScreenPos() const;// (0,0) is at top left
    Vec2 GetCursorWindowPos() const;
    Vec2 GetCursorDelta() const;
    Vec2 GetCursorWindowPosFlipY() const;// (0,0) is at bottom left
    void SetCursorScreenPos( const Vec2& pos );
    void SetCursorWindowPos( const Vec2& pos );

    void LockCursor( bool lock );
    void ShowCursor( bool show );
    void ClipCursor( bool clip );
    bool IsCursorLocked() { return m_cursorSettings.lock; };
    bool IsCursorVisible() { return m_cursorSettings.show; };
    bool IsCursorCliped() { return m_cursorSettings.clip; };

    void ShowAndUnlockCursor();
    void PushCursorSettings();
    void PopCursorSettings();

    void UpdateClipCursorAfterResize();

    void SetWindowHasFocus( bool focus );
    bool WindowHasFocus() { return m_hasFocus; };
    bool DidWindowJustLoseFocus();
    bool DidWindowJustGainFocus();


    XboxController& GetController( int controllerID ) { return m_controllers[controllerID]; }
    XboxController* GetFirstConnectedController();

    // Attach Observers
    void Attach( InputObserver* inputObserver );
    void Detach( InputObserver* inputObserver );

public:
    static const int NUM_KEYS = 256; // Values match Windows VK_XXX virtual keys; common keys are 'A' and so on.
    static const int NUM_CONTROLLERS = 4; // Limit imposed by XInput

    static const unsigned char MOUSE_LEFT;
    static const unsigned char MOUSE_RIGHT;
    static const unsigned char MOUSE_MIDDLE;

    static const unsigned char KEYBOARD_ESCAPE;
    static const unsigned char KEYBOARD_F1;
    static const unsigned char KEYBOARD_F2;
    static const unsigned char KEYBOARD_F3;
    static const unsigned char KEYBOARD_F4;
    static const unsigned char KEYBOARD_F5;
    static const unsigned char KEYBOARD_F6;
    static const unsigned char KEYBOARD_F7;
    static const unsigned char KEYBOARD_F8;
    static const unsigned char KEYBOARD_F9;
    static const unsigned char KEYBOARD_F10;
    static const unsigned char KEYBOARD_F11;
    static const unsigned char KEYBOARD_F12;
    static const unsigned char KEYBOARD_SEMICOLON;
    static const unsigned char KEYBOARD_PLUS;
    static const unsigned char KEYBOARD_COMMA;
    static const unsigned char KEYBOARD_MINUS;
    static const unsigned char KEYBOARD_PERIOD;
    static const unsigned char KEYBOARD_FORWARD_SLASH;
    static const unsigned char KEYBOARD_TILDE;
    static const unsigned char KEYBOARD_L_BRACKET;
    static const unsigned char KEYBOARD_BACK_SLASH;
    static const unsigned char KEYBOARD_R_BRACKET;
    static const unsigned char KEYBOARD_QUOTE;
    static const unsigned char KEYBOARD_BACKSPACE;
    static const unsigned char KEYBOARD_TAB;
    static const unsigned char KEYBOARD_ENTER;
    static const unsigned char KEYBOARD_SHIFT;
    static const unsigned char KEYBOARD_CONTROL;
    static const unsigned char KEYBOARD_ALT;
    static const unsigned char KEYBOARD_PAUSE;
    static const unsigned char KEYBOARD_CAPSLOCK;
    static const unsigned char KEYBOARD_L_SHIFT;
    static const unsigned char KEYBOARD_R_SHIFT;
    static const unsigned char KEYBOARD_L_CONTROL;
    static const unsigned char KEYBOARD_R_CONTROL;
    static const unsigned char KEYBOARD_L_ALT;
    static const unsigned char KEYBOARD_R_ALT;
    static const unsigned char KEYBOARD_SPACE;
    static const unsigned char KEYBOARD_PAGEUP;
    static const unsigned char KEYBOARD_PAGEDOWN;
    static const unsigned char KEYBOARD_END;
    static const unsigned char KEYBOARD_HOME;
    static const unsigned char KEYBOARD_SCREENSHOT;
    static const unsigned char KEYBOARD_INSERT;
    static const unsigned char KEYBOARD_DELETE;
    static const unsigned char KEYBOARD_NUMPAD0;
    static const unsigned char KEYBOARD_NUMPAD1;
    static const unsigned char KEYBOARD_NUMPAD2;
    static const unsigned char KEYBOARD_NUMPAD3;
    static const unsigned char KEYBOARD_NUMPAD4;
    static const unsigned char KEYBOARD_NUMPAD5;
    static const unsigned char KEYBOARD_NUMPAD6;
    static const unsigned char KEYBOARD_NUMPAD7;
    static const unsigned char KEYBOARD_NUMPAD8;
    static const unsigned char KEYBOARD_NUMPAD9;
    static const unsigned char KEYBOARD_MULTIPLY;
    static const unsigned char KEYBOARD_ADD;
    static const unsigned char KEYBOARD_SEPARATOR;
    static const unsigned char KEYBOARD_SUBTRACT;
    static const unsigned char KEYBOARD_DECIMAL;
    static const unsigned char KEYBOARD_DIVIDE;
    static const unsigned char KEYBOARD_UP_ARROW;
    static const unsigned char KEYBOARD_LEFT_ARROW;
    static const unsigned char KEYBOARD_DOWN_ARROW;
    static const unsigned char KEYBOARD_RIGHT_ARROW;




protected:
    void ClearKeyboardJustChangedFlags();
    void ClearControllerJustChangedFlags();
    void UpdateController( int controllerID );

    struct CursorSettings
    {
        bool lock = false;
        bool clip = false;
        bool show = true;
    };

    CursorSettings m_cursorSettings;

    KeyButtonState m_keyStates[NUM_KEYS];
    XboxController m_controllers[NUM_CONTROLLERS];
    int m_mouseWheelDelta = 0;

    Vec2 m_cursorPrevWindowPos;
    Vec2 m_cursorCurrWindowPos;

    vector<InputObserver*> m_inputObservers;
    Window* m_window;

    // window
    bool m_hasFocus = true;
    bool m_justLostFocus = false;
    bool m_justGainedFocus = false;


    std::stack<CursorSettings> m_cursorModeStack;
};