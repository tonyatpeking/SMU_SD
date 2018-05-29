#include "Engine/Core/Blackboard.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IRange.hpp"
#include "Engine/Math/Range.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Core/ErrorUtils.hpp"

#include "ThirdParty/pugixml/pugixml.hpp"


void Blackboard::PopulateFromXmlElementAttributes( const XMLElement& element )
{
    for( XMLAttribute attribute = element.FirstAttribute(); attribute.Valid();
         attribute = attribute.NextAttribute() )
    {
        SetValue( attribute.Name(), attribute.Value() );
    }

    for( XMLElement child = element.FirstChild(); child.Valid();
         child = child.NextSibling() )
    {
        for( XMLAttribute attribute = child.FirstAttribute(); attribute.Valid();
             attribute = attribute.NextAttribute() )
        {
            SetValue( attribute.Name(), attribute.Value() );
        }
    }
}

void Blackboard::PopulateFromFile( const String& path )
{
    XMLDocument doc;
    doc.LoadFromFile( path );
    XMLElement xmlConfig = doc.FirstChild();
    PopulateFromXmlElementAttributes( xmlConfig );
}

void Blackboard::SetValue( const String& keyName, const String& newValue )
{
    if( m_keyValuePairs.find( keyName ) != m_keyValuePairs.end() )
    {
        LOG_WARNING( Stringf( "Overwritting Blackboard entry %s:%s with %s:%s" ,
                                   keyName.c_str(), m_keyValuePairs[keyName].c_str(),
                                   keyName.c_str(), newValue.c_str() ) );
    }
    m_keyValuePairs[keyName] = newValue;
}

bool Blackboard::GetValue( const String& keyName, bool defaultValue ) const
{
    auto iter = m_keyValuePairs.find( keyName );

    if( iter == m_keyValuePairs.end() )
        return defaultValue;

    bool out_bool = false;
    ParseStatus status = SetFromString( iter->second, out_bool );

    if( status == PARSE_SUCCESS )
        return out_bool;

    return defaultValue;
}

int Blackboard::GetValue( const String& keyName, int defaultValue ) const
{
    auto iter = m_keyValuePairs.find( keyName );

    if( iter == m_keyValuePairs.end() )
        return defaultValue;

    int out_int = 0;
    ParseStatus status = SetFromString( iter->second, out_int );

    if( status == PARSE_SUCCESS )
        return out_int;

    return defaultValue;
}

float Blackboard::GetValue( const String& keyName, float defaultValue ) const
{
    auto iter = m_keyValuePairs.find( keyName );

    if( iter == m_keyValuePairs.end() )
        return defaultValue;

    float out_float = 0;
    ParseStatus status = SetFromString( iter->second, out_float );

    if( status == PARSE_SUCCESS )
        return out_float;

    return defaultValue;
}

String Blackboard::GetValue( const String& keyName, const String& defaultValue ) const
{
    auto iter = m_keyValuePairs.find( keyName );

    if( iter == m_keyValuePairs.end() )
        return defaultValue;

    return iter->second;
}

String Blackboard::GetValue( const String& keyName, const char* defaultValue ) const
{
    auto iter = m_keyValuePairs.find( keyName );

    if( iter == m_keyValuePairs.end() )
        return defaultValue;

    return iter->second;
}

Rgba Blackboard::GetValue( const String& keyName, const Rgba& defaultValue ) const
{
    auto iter = m_keyValuePairs.find( keyName );

    if( iter == m_keyValuePairs.end() )
        return defaultValue;

    Rgba color;
    ParseStatus status = color.SetFromText( iter->second );

    if( status == PARSE_SUCCESS )
        return color;

    return defaultValue;
}

Vec2 Blackboard::GetValue( const String& keyName, const Vec2& defaultValue ) const
{
    auto iter = m_keyValuePairs.find( keyName );

    if( iter == m_keyValuePairs.end() )
        return defaultValue;

    Vec2 vec2;
    ParseStatus status = vec2.SetFromText( iter->second );

    if( status == PARSE_SUCCESS )
        return vec2;

    return defaultValue;
}

IVec2 Blackboard::GetValue( const String& keyName, const IVec2& defaultValue ) const
{
    auto iter = m_keyValuePairs.find( keyName );

    if( iter == m_keyValuePairs.end() )
        return defaultValue;

    IVec2 intVec2;
    ParseStatus status = intVec2.SetFromText( iter->second );

    if( status == PARSE_SUCCESS )
        return intVec2;

    return defaultValue;
}

Range Blackboard::GetValue( const String& keyName, const Range& defaultValue ) const
{
    auto iter = m_keyValuePairs.find( keyName );

    if( iter == m_keyValuePairs.end() )
        return defaultValue;

    Range floatRange;
    ParseStatus status = floatRange.SetFromText( iter->second );

    if( status == PARSE_SUCCESS )
        return floatRange;

    return defaultValue;
}

IRange Blackboard::GetValue( const String& keyName, const IRange& defaultValue ) const
{
    auto iter = m_keyValuePairs.find( keyName );

    if( iter == m_keyValuePairs.end() )
        return defaultValue;

    IRange intRange;
    ParseStatus status = intRange.SetFromText( iter->second );

    if( status == PARSE_SUCCESS )
        return intRange;

    return defaultValue;
}

char Blackboard::GetValue( const String& keyName, char defaultValue ) const
{
    auto iter = m_keyValuePairs.find( keyName );

    if( iter == m_keyValuePairs.end() )
        return defaultValue;

    if( iter->second == "" )
        return defaultValue;

    return iter->second[0];
}
