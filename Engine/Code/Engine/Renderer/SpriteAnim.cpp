#include "Engine/Renderer/SpriteAnim.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"

SpriteAnim::SpriteAnim( const SpriteAnimDefinition* definition )
    : m_definition( definition )
{

}

void SpriteAnim::Update( float deltaSeconds )
{
    if( !m_isPlaying )
        return;
    m_elapsedSeconds += deltaSeconds;
    if( m_elapsedSeconds > GetDurationSeconds() )
    {
        if( Definition()->m_playbackMode == SpriteAnimMode::PLAY_TO_END )
        {
            m_isPlaying = false;
            m_isFinished = true;
        }
        if( Definition()->m_playbackMode == SpriteAnimMode::LOOPING )
        {
            SetFractionElapsed( 0.f );
        }
    }
}

AABB2 SpriteAnim::GetCurrentUVs() const
{
    return GetSpriteSheet()->GetUVsForSpriteIndex( GetCurrentSpriteIdx() );
}

SpriteAnimType SpriteAnim::GetAnimType() const
{
    return Definition()->m_animType;
}

const Texture* SpriteAnim::GetTexture() const
{
    return GetSpriteSheet()->GetTexture();
}

void SpriteAnim::Reset()
{
    m_elapsedSeconds = 0.f;
}

float SpriteAnim::GetSecondsRemaining() const
{
    return GetDurationSeconds() - m_elapsedSeconds;
}

float SpriteAnim::GetFractionElapsed() const
{
    if( GetDurationSeconds() == 0.f )
        return 0.f;
    return m_elapsedSeconds / GetDurationSeconds();
}

float SpriteAnim::GetFractionRemaining() const
{
    return 1.f - GetFractionElapsed();
}

void SpriteAnim::SetFractionElapsed( float fractionElapsed )
{
    m_elapsedSeconds = GetDurationSeconds() * fractionElapsed;
}

float SpriteAnim::GetDurationSeconds() const
{
    return Definition()->m_duration;
}

float SpriteAnim::GetFPS() const
{
    return Definition()->m_fps;
}

const SpriteSheet* SpriteAnim::GetSpriteSheet() const
{
    return Definition()->m_spriteSheet;
}

int SpriteAnim::GetCurrentSpriteIdx() const
{
    int IdxOfSpriteIdx = Lerp( 0, (int) Definition()->m_spriteIndices.size() - 1, GetFractionElapsed() );
    return  Definition()->m_spriteIndices[IdxOfSpriteIdx];
}
