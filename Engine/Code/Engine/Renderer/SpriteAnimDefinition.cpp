#include "Engine/String/StringUtils.hpp"
#include "Engine/FileIO/XmlUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Renderer.hpp"

SpriteAnimDefinition::SpriteAnimDefinition( const XMLElement& xmlDef, Renderer& renderer )
{
    XMLElement parentXMLDef = xmlDef.Parent();

    LOAD_XML_ATTRIBUTE( xmlDef, m_animType );
    LOAD_XML_ATTRIBUTE( xmlDef, m_playbackMode );

    LOAD_XML_ATTRIBUTE( xmlDef, m_spritePath );
    LOAD_XML_ATTRIBUTE( xmlDef, m_spriteLayout );
    if( m_spritePath == "" ) // load sprite sheet from parent if it is missing
    {
        LOAD_XML_ATTRIBUTE( parentXMLDef, m_spritePath );
        LOAD_XML_ATTRIBUTE( parentXMLDef, m_spriteLayout );
    }
    if( m_spritePath == "" ) // load sprite sheet from parent if it is missing
    {
        LOG_WARNING( "Could not find attribute [spritePath] when parsing SpriteAnim xml" );
    }
    m_spriteSheet = renderer.CreateOrGetSpriteSheet( m_spritePath, m_spriteLayout );
    if( !m_spriteSheet )
        LOG_WARNING(  "Failed to create spritesheet at path: " + m_spritePath );

    LOAD_XML_ATTRIBUTE( xmlDef, m_spriteIndices );
    if( m_spriteIndices.size() == 0 ) // load sprite indices from start and end
    {
        LOAD_XML_ATTRIBUTE( xmlDef, m_spriteIndexRange );
        FillSpriteIndicesFromRange();
    }

    LOAD_XML_ATTRIBUTE( xmlDef, m_duration );
    LOAD_XML_ATTRIBUTE( xmlDef, m_fps );
    if( m_duration == 0.f && m_fps == 0.f ) // load duration/fps from parent if both are missing
    {
        LOAD_XML_ATTRIBUTE( parentXMLDef, m_duration );
        LOAD_XML_ATTRIBUTE( parentXMLDef, m_fps );
    }
    if( m_duration == 0.f && m_fps != 0.f )
    {
        SetDurationFromFPS();
    }
    else if( m_duration != 0.f && m_fps == 0.f )
    {
        SetFPSFromDuration();
    }
    else if( m_duration == 0.f && m_fps == 0.f )
    {
        //LOG_WARNING( "Both duration and fps are 0 for SpriteAnimDefinition, set only one to non zero!" );
    }
    else
    {
        LOG_WARNING( "Both duration and fps are being set for SpriteAnimDefinition, fps will be overwritten!" );
        SetFPSFromDuration();
    }

    LOAD_XML_ATTRIBUTE( xmlDef, m_autoOrient );
}

const Texture* SpriteAnimDefinition::GetTexture() const
{
    return m_spriteSheet->GetTexture();
}

void SpriteAnimDefinition::SwapSpriteSheet( const String& spriteSheetPath, Renderer& renderer )
{
    m_spritePath = spriteSheetPath;
    m_spriteSheet = renderer.CreateOrGetSpriteSheet( m_spritePath, m_spriteLayout );
    if( !m_spriteSheet )
        LOG_WARNING( "Failed to create spritesheet at path: " + m_spritePath );
}

void SpriteAnimDefinition::FillSpriteIndicesFromRange()
{
    m_spriteIndices.clear();
    int endIdx = m_spriteIndexRange.min;
    int startIdx = m_spriteIndexRange.max;
    if( startIdx == endIdx )
    {
        m_spriteIndices.push_back( startIdx );
        return;
    }

    int spriteIdxIncrement =  Sign( endIdx - startIdx );

    for( int spriteIdx = startIdx; spriteIdx != endIdx; spriteIdx += spriteIdxIncrement )
    {
        m_spriteIndices.push_back( spriteIdx );
    }
    m_spriteIndices.push_back( endIdx );
}

void SpriteAnimDefinition::SetDurationFromFPS()
{
    if( m_fps == 0.f )
    {
        m_duration = 0.f;
        return;
    }

    m_duration = (float) m_spriteIndices.size() / m_fps;
}

void SpriteAnimDefinition::SetFPSFromDuration()
{
    if( m_duration == 0.f )
    {
        m_fps = 0.f;
        return;
    }

    m_fps = (float) m_spriteIndices.size() / m_duration;
}

SpriteAnimMode StringToSpriteAnimMode( const String& spriteAnimModeStr )
{
    if( spriteAnimModeStr == STRINGIZE( PLAY_TO_END ) )
        return SpriteAnimMode::PLAY_TO_END;

    if( spriteAnimModeStr == STRINGIZE( LOOPING ) )
        return SpriteAnimMode::LOOPING;

    return SpriteAnimMode::INVALID;
}

const SpriteAnimMode ParseXmlAttribute( const XMLElement& element, const char* attributeName, const SpriteAnimMode defaultValue )
{
    String stringValue = ParseXmlAttribute( element, attributeName, "" );
    SpriteAnimMode animMode = StringToSpriteAnimMode( stringValue );
    if( animMode != SpriteAnimMode::INVALID )
        return animMode;

    return defaultValue;
}
