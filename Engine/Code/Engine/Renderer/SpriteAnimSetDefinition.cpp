#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/FileIO/XmlUtils.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"

SpriteAnimSetDefinition::SpriteAnimSetDefinition( const XMLElement& xmlAnimSetDef, Renderer& renderer )
{
    for( XMLElement xmlAnimDef = xmlAnimSetDef.FirstChild(); xmlAnimDef.Valid(); xmlAnimDef = xmlAnimDef.NextSibling() )
    {
        if( xmlAnimDef.Name() != STRINGIZE( SpriteAnim ) )
        {
            LOG_WARNING( "Unexpected element found in SpriteAnimSet: " + xmlAnimDef.Name() );
            LOG_WARNING( "Expected: " + STRINGIZE( SpriteAnim ) );
            continue;
        }
        SpriteAnimDefinition* animDef = new SpriteAnimDefinition( xmlAnimDef, renderer );
        AddAnimDef( animDef );
    }
}

SpriteAnimSetDefinition::~SpriteAnimSetDefinition()
{
    for( const auto& animDefPair : m_animDefs )
    {
        delete animDefPair.second;
    }
    m_animDefs.clear();
}

void SpriteAnimSetDefinition::SwapSpriteSheet( const String& spriteSheetPath, Renderer& renderer )
{
    for( auto& animDef : m_animDefs )
    {
        animDef.second->SwapSpriteSheet( spriteSheetPath, renderer );
    }
}

void SpriteAnimSetDefinition::AddAnimDef( SpriteAnimDefinition* animDef )
{
    SpriteAnimType animType = animDef->m_animType;
    if( m_animDefs.find( animType ) != m_animDefs.end() )
    {
        delete m_animDefs[animType];
    }
    m_animDefs[animType] = animDef;
}

