#pragma once
#include <functional>
#include <vector>
#include "Engine/Time/Tween.hpp"
#include "Engine/Core/EngineCommonH.hpp"

//Important! By default Tweens will auto self delete when duration is up!

class TweenSystem
{
public:
	TweenSystem(){};
	~TweenSystem();

    void Update( float ds );

    template<typename T>
    Tween* MakeTween( T& val, const T& end, float duration)
    {
        Tween* tween = new TweenT<T>( val, end, duration );
        m_tweens.push_back( tween );
        tween->m_tweenSystem = this;
        return tween;
    }

    template<typename T>
    Tween* MakeTween( std::function<void( const T& )> setter, const T& start,
                      const T& end, float duration)
    {
        Tween* tween = new TweenT<T>( setter, start, end, duration );
        m_tweens.push_back( tween );
        tween->m_tweenSystem = this;
        return tween;
    }

    void ClearAll();
    void RemoveTween( Tween* tween );
    uint GetTweenCount() { return (uint) m_tweens.size(); };
private:
    vector<Tween*> m_tweens;
};
