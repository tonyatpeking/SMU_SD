#include "Engine/Renderer/SpriteAnimSet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
#include "Engine/Renderer/SpriteAnim.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/ErrorUtils.hpp"


SpriteAnimSet::SpriteAnimSet( SpriteAnimSetDefinition* animSetDef )
    : m_animSetDef( animSetDef )
{
    m_anims.clear();
    for ( const auto& animDefPair : m_animSetDef->m_animDefs )
    {
        SpriteAnimType animType = animDefPair.first;
        m_anims[animType] = new SpriteAnim( animDefPair.second );
    }
    SetCurrentAnim( m_animSetDef->m_defaultAnimType );
}

SpriteAnimSet::~SpriteAnimSet()
{
    for( const auto& animPair : m_anims )
    {
        delete animPair.second;
    }
    m_anims.clear();
}

void SpriteAnimSet::Update( float deltaSeconds )
{
    m_currentAnim->Update( deltaSeconds );
}

void SpriteAnimSet::StartAnim( SpriteAnimType animType )
{
    SetCurrentAnim( animType );
    m_currentAnim->Resume();
}

void SpriteAnimSet::SetCurrentAnim( SpriteAnimType animType )
{
    if( m_anims.find( animType ) != m_anims.end() )
        m_currentAnim = m_anims[animType];
    else
        LOG_WARNING( "could not find SpriteAnimType" );
}

const Texture* SpriteAnimSet::GetCurrentTexture() const
{
    return m_currentAnim->GetTexture();
}

AABB2 SpriteAnimSet::GetCurrentUVs() const
{
    return m_currentAnim->GetCurrentUVs();
}

bool SpriteAnimSet::HasAnim( SpriteAnimType animType )
{
    return m_anims.find( animType ) != m_anims.end();
}
