#pragma once
#include "Engine/Input/KeyButtonState.hpp"

class Vec2;

class AnalogJoystick
{
public:
    AnalogJoystick() {};
    ~AnalogJoystick() {};

    // Notification Methods ( notify the joystick of movement )
    void OnChangePosition( short rawXShort, short rawYShort );
    void Clear();

    // Query Methods
    Vec2 GetPosition() const;
    float GetMagnitude() const;
    float GetAngleDegrees() const;
public:

    float m_innerDeadZoneFraction = 0.25f;
    float m_outerDeadZoneFraction = 0.95f;

    float m_rawXNormalized;
    float m_rawYNormalized;
    float m_correctedX;
    float m_correctedY;
    float m_correctedMagnitude;
    float m_angleDegrees;

};