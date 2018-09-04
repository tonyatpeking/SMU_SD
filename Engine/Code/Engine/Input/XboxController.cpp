#include "Engine/Input/XboxController.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include <windows.h>			// #include this ( massive, platform-specific ) header in very few places
#include <Xinput.h> // include the Xinput API
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use 9_1_0 explicitly for broadest compatibility



const unsigned short XboxController::XINPUT_KEYMAPPING[] ={
    XINPUT_GAMEPAD_DPAD_UP,
    XINPUT_GAMEPAD_DPAD_DOWN,
    XINPUT_GAMEPAD_DPAD_LEFT,
    XINPUT_GAMEPAD_DPAD_RIGHT,
    XINPUT_GAMEPAD_START,
    XINPUT_GAMEPAD_BACK,
    XINPUT_GAMEPAD_LEFT_THUMB,
    XINPUT_GAMEPAD_RIGHT_THUMB,
    XINPUT_GAMEPAD_LEFT_SHOULDER,
    XINPUT_GAMEPAD_RIGHT_SHOULDER,
    XINPUT_GAMEPAD_A,
    XINPUT_GAMEPAD_B,
    XINPUT_GAMEPAD_X,
    XINPUT_GAMEPAD_Y
};

const unsigned char XboxController::XBOX_KEY_UP             = 0;
const unsigned char XboxController::XBOX_KEY_DOWN           = 1;
const unsigned char XboxController::XBOX_KEY_LEFT           = 2;
const unsigned char XboxController::XBOX_KEY_RIGHT          = 3;
const unsigned char XboxController::XBOX_KEY_START          = 4;
const unsigned char XboxController::XBOX_KEY_BACK           = 5;
const unsigned char XboxController::XBOX_KEY_LEFT_THUMB     = 6;
const unsigned char XboxController::XBOX_KEY_RIGHT_THUMB    = 7;
const unsigned char XboxController::XBOX_KEY_LEFT_SHOULDER  = 8;
const unsigned char XboxController::XBOX_KEY_RIGHT_SHOULDER = 9;
const unsigned char XboxController::XBOX_KEY_A              = 10;
const unsigned char XboxController::XBOX_KEY_B              = 11;
const unsigned char XboxController::XBOX_KEY_X              = 12;
const unsigned char XboxController::XBOX_KEY_Y              = 13;

XboxController::XboxController( int controllerID )
    : m_controllerID( controllerID )
{
}


void XboxController::OnKeyIsDown( unsigned char keyCode )
{
    if( !m_keyStates[keyCode].m_isDown )
    {
        m_keyStates[keyCode].m_isDown = true;
        m_keyStates[keyCode].m_justPressed = true;
    }
}

void XboxController::UpdateLeftTrigger( unsigned char magnitudeUchar )
{
    m_leftTriggerNormalized = (float) magnitudeUchar * INVERSE_MAX_UCHAR;
}

void XboxController::UpdateRightTrigger( unsigned char magnitudeUchar )
{
    m_rightTriggerNormalized = (float) magnitudeUchar * INVERSE_MAX_UCHAR;
}

void XboxController::OnKeyIsUp( unsigned char keyCode )
{
    if( m_keyStates[keyCode].m_isDown )
    {
        m_keyStates[keyCode].m_isDown = false;
        m_keyStates[keyCode].m_justReleased = true;
    }
}

void XboxController::UpdateButtons( unsigned short wButtons )
{
    for( unsigned char keyCode = 0; keyCode < NUM_KEYS; ++keyCode )
    {
        // Extract each button state from wButtons
        bool isButtonDown = ( wButtons & XINPUT_KEYMAPPING[keyCode] ) == XINPUT_KEYMAPPING[keyCode];
        if( isButtonDown )
        {
            OnKeyIsDown( keyCode );
        }
        else
        {
            OnKeyIsUp( keyCode );
        }
    }
}

bool XboxController::IsKeyPressed( unsigned char keyCode ) const
{
    return m_keyStates[keyCode].m_isDown;
}

bool XboxController::WasKeyJustPressed( unsigned char keyCode ) const
{
    return m_keyStates[keyCode].m_justPressed;
}

bool XboxController::WasKeyJustReleased( unsigned char keyCode ) const
{
    return m_keyStates[keyCode].m_justReleased;
}

bool XboxController::WasAnyKeyJustPressed() const
{
    for( int keyCode = 0; keyCode < XboxController::NUM_KEYS; ++keyCode )
    {
        if( WasKeyJustPressed( (unsigned char) keyCode ) )
            return true;
    }
    return false;
}

float XboxController::GetLeftTriggerNormalized() const
{
    return m_leftTriggerNormalized;
}

float XboxController::GetRightTriggerNormalized() const
{
    return m_rightTriggerNormalized;
}

void XboxController::UpdateIsConnected( bool isConnected )
{
    if( m_isConnected && !isConnected ) //was connected but not anymore
    {
        ClearAllKeys();
    }
    m_isConnected = isConnected;
}

void XboxController::ClearJustChangedFlags()
{
    for( int keyCode = 0; keyCode < XboxController::NUM_KEYS; ++keyCode )
    {
        m_keyStates[keyCode].m_justPressed = false;
        m_keyStates[keyCode].m_justReleased = false;
    }
}

void XboxController::ClearAllKeys()
{
    for( int keyCode = 0; keyCode < XboxController::NUM_KEYS; ++keyCode )
    {
        m_keyStates[keyCode].Clear();
    }
    for( int joystickID = 0; joystickID < NUM_JOYSTICKS; ++joystickID )
    {
        m_joySticks[joystickID].Clear();
    }
}


