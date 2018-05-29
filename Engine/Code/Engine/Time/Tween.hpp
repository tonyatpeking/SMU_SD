#pragma once
#include <functional>
#include "Engine/Core/SmartEnum.hpp"

class TweenSystem;

SMART_ENUM
(
    TweenMode,

    PLAY_ONCE_DELETE,
    PLAY_ONCE_DONT_DELETE,
    LOOPING,
    PINGPONG
)

class Tween
{
    friend class TweenSystem;
public:
    void Update( float ds );
    void SetEasing( std::function<float( float )> easing ) { m_easing = easing; };
    void SetTweenMode( TweenMode tweenMode ) { m_tweenMode = tweenMode; };
    void SetEndCallback( std::function<void( void )> endCallback ) { m_endCallback = endCallback; };
    float GetDuration() { return m_duration; };
    float GetElapsedTime() { return m_elapsedTime; };
    bool IsComplete() { return !m_isPlaying; }; //only use this if TweenMode is PLAY_ONCE_DONT_DELETE
    virtual ~Tween() {};
protected:
    virtual void UpdateValue( float tLerp ) = 0;
    float m_duration = 0;
    float m_elapsedTime = 0;
    float m_direction = 1.f; //used for pingpong
    bool m_isPlaying = true;
    TweenMode m_tweenMode = TweenMode::PLAY_ONCE_DELETE;
    std::function<float( float )> m_easing = nullptr;
    std::function<void( void )> m_endCallback;
    TweenSystem* m_tweenSystem;
};

template<typename T>
class TweenT : public Tween
{
    friend class TweenSystem;
public:


protected:
    void UpdateValue( float tLerp ) override
    {
        if( m_setter )
            m_setter( Lerp( m_startValue, m_endValue, tLerp ) );
        else
            m_valueRef = Lerp( m_startValue, m_endValue, tLerp );
    }

    //TweenSystem is in charge of creation and deletion
    TweenT( T& val, const T& end, float duration,
            std::function<void()> endCallback = nullptr )
        : m_valueRef( val )
        , m_startValue( val )
        , m_endValue( end )
    {
        m_duration = duration;
        m_endCallback = endCallback;
    }

    TweenT( std::function<void( const T& )> setter,
            const T& start,
            const T& end,
            float duration,
            std::function<void()> endCallback = nullptr )
        : m_startValue( start )
        , m_endValue( end )
        , m_valueRef( m_startValue ) // not used
    {
        m_setter = setter;
        m_duration = duration;
        m_endCallback = endCallback;
    }

    ~TweenT() {};

    T m_startValue;
    T m_endValue;
    T& m_valueRef;
    std::function<void( const T& )> m_setter = nullptr;

};

