#include "Engine/Time/TweenSystem.hpp"
#include "Engine/Core/ContainerUtils.hpp"


TweenSystem::~TweenSystem()
{
    ClearAll();
}

void TweenSystem::Update( float ds )
{
    for ( auto& tween : m_tweens )
    {
        tween->Update(ds);
    }
    for (int tweenIdx = (int)m_tweens.size() - 1; tweenIdx >= 0 ; --tweenIdx)
    {
        Tween* tweenPtr = m_tweens[tweenIdx];
        if( tweenPtr->m_tweenMode != TweenMode::PLAY_ONCE_DELETE )
            continue;
        if( tweenPtr->GetElapsedTime() >= tweenPtr->GetDuration() )
            RemoveTween( tweenPtr );
    }
}

void TweenSystem::ClearAll()
{
    ContainerUtils::DeletePointers( m_tweens );
}

void TweenSystem::RemoveTween( Tween* tween )
{
    ContainerUtils::EraseOneValue( m_tweens, tween );
    delete tween;
}
