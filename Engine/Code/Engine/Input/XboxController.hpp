#pragma once
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/AnalogJoystick.hpp"

class XboxController
{
public:
    XboxController( int controllerID );
    ~XboxController() {};

    // Notification Methods ( notify the input system of a key press )
    void OnKeyIsDown( unsigned char keyCode );
    void UpdateLeftTrigger( unsigned char magnitudeUchar );
    void UpdateRightTrigger( unsigned char magnitudeUchar );
    void OnKeyIsUp( unsigned char keyCode );
    void UpdateButtons( unsigned short wButtons );

    // Query Methods
    bool IsConnected() const { return m_isConnected; };
    bool IsKeyPressed( unsigned char keyCode ) const;
    bool WasKeyJustPressed( unsigned char keyCode ) const;
    bool WasKeyJustReleased( unsigned char keyCode ) const;
    bool WasAnyKeyJustPressed() const;
    float GetLeftTriggerNormalized() const;
    float GetRightTriggerNormalized() const;


    AnalogJoystick& GetJoystick( int joystickID ) { return m_joySticks[joystickID]; }
    void UpdateIsConnected( bool isConnected );
    void ClearJustChangedFlags();
    void ClearAllKeys();

public:

    static const int NUM_KEYS = 14;
    static const int NUM_JOYSTICKS = 2;

    static const int LEFT_JOYSTICK = 0;
    static const int RIGHT_JOYSTICK = 1;

    static const unsigned short XINPUT_KEYMAPPING[NUM_KEYS]; // Maps keyCode to XINPUT wButtons

    static const unsigned char XBOX_KEY_UP;
    static const unsigned char XBOX_KEY_DOWN;
    static const unsigned char XBOX_KEY_LEFT;
    static const unsigned char XBOX_KEY_RIGHT;
    static const unsigned char XBOX_KEY_START;
    static const unsigned char XBOX_KEY_BACK;
    static const unsigned char XBOX_KEY_LEFT_THUMB;
    static const unsigned char XBOX_KEY_RIGHT_THUMB;
    static const unsigned char XBOX_KEY_LEFT_SHOULDER;
    static const unsigned char XBOX_KEY_RIGHT_SHOULDER;
    static const unsigned char XBOX_KEY_A;
    static const unsigned char XBOX_KEY_B;
    static const unsigned char XBOX_KEY_X;
    static const unsigned char XBOX_KEY_Y;


    int m_controllerID;
    bool m_isConnected = false;

protected:

    KeyButtonState m_keyStates[NUM_KEYS];
    AnalogJoystick m_joySticks[NUM_JOYSTICKS];
    float m_leftTriggerNormalized = 0;
    float m_rightTriggerNormalized = 0;


};