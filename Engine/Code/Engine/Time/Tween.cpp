#include "Engine/Time/Tween.hpp"



void Tween::Update( float ds )
{
    if( !m_isPlaying )
        return;
    m_elapsedTime += ds;
    float tNormalized = 0.f;

    if( m_duration == 0.f )
        tNormalized = 1.f;
    else
        tNormalized = m_elapsedTime / m_duration;

    if( tNormalized > 1.f )
        tNormalized = 1.f;

    if( m_easing )
        tNormalized = m_easing( tNormalized );

    if( m_direction < 0 )
        tNormalized = 1.f - tNormalized;
    UpdateValue( tNormalized );

    if( m_elapsedTime >= m_duration )
    {
        if( m_endCallback )
            m_endCallback();
        if( m_tweenMode == TweenMode::PLAY_ONCE_DONT_DELETE )
            m_isPlaying = false;
        if( m_tweenMode == TweenMode::LOOPING )
            m_elapsedTime = 0;
        if( m_tweenMode == TweenMode::PINGPONG )
        {
            m_elapsedTime = 0;
            m_direction = -m_direction;
        }
    }
}
