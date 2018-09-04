#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommonH.hpp"


void AnalogJoystick::OnChangePosition( short rawXShort, short rawYShort )
{
    m_rawXNormalized = (float) rawXShort * INVERSE_MAX_SHORT;
    m_rawYNormalized = (float) rawYShort * INVERSE_MAX_SHORT;

    Vec2 rawPosition = Vec2( m_rawXNormalized, m_rawYNormalized );
    float rawMagnitude = rawPosition.GetLength();
    m_angleDegrees = rawPosition.GetOrientationDegrees();

    //remap magnitude to exclude dead zones
    m_correctedMagnitude = RangeMapFloat( rawMagnitude
                                          , m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0, 1 );
    m_correctedMagnitude = Clampf01( m_correctedMagnitude );

    m_correctedX = m_correctedMagnitude * CosDeg( m_angleDegrees );
    m_correctedY = m_correctedMagnitude * SinDeg( m_angleDegrees );
}

void AnalogJoystick::Clear()
{
    m_rawXNormalized = 0.f;
    m_rawYNormalized = 0.f;
    m_correctedX = 0.f;
    m_correctedY = 0.f;
    m_correctedMagnitude = 0.f;
    m_angleDegrees =  0.f;
}

Vec2 AnalogJoystick::GetPosition() const
{
    return Vec2( m_correctedX, m_correctedY );
}

float AnalogJoystick::GetMagnitude() const
{
    return m_correctedMagnitude;
}

float AnalogJoystick::GetAngleDegrees() const
{
    return m_angleDegrees;
}
