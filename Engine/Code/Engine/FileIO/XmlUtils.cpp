

#include "Engine/FileIO/XmlUtils.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IRange.hpp"
#include "Engine/Math/Range.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/ErrorUtils.hpp"


String XMLAttribute::Name() const
{
    return m_attribute.name();
}

String XMLAttribute::Value( const char* defaultValue/*=nullptr */ )
{
    String stringValue = m_attribute.value();

    if( !stringValue.empty() )
        return stringValue;
    else if( defaultValue == nullptr )
        return "";
    else
        return String( defaultValue );
}

String XMLAttribute::Value( const String& defaultValue )
{
    String stringValue = m_attribute.value();

    if( !stringValue.empty() )
        return stringValue;
    else
        return defaultValue;
}

int XMLAttribute::Value( int defaultValue )
{
    String stringValue = m_attribute.value();
    int out_int;
    ParseStatus status = SetFromString( stringValue, out_int );

    if( status == PARSE_SUCCESS )
        return out_int;
    else
        return defaultValue;
}

char XMLAttribute::Value( char defaultValue )
{
    String stringValue = m_attribute.value();

    if( stringValue.length() > 1 )
        return stringValue[0];
    else
        return defaultValue;
}

bool XMLAttribute::Value( bool defaultValue )
{
    String stringValue = m_attribute.value();

    bool out_bool;
    ParseStatus status = SetFromString( stringValue, out_bool );

    if( status == PARSE_SUCCESS )
        return out_bool;
    else
        return defaultValue;
}

float XMLAttribute::Value( float defaultValue )
{
    String stringValue = m_attribute.value();

    float out_float;
    ParseStatus status = SetFromString( stringValue, out_float );

    if( status == PARSE_SUCCESS )
        return out_float;
    else
        return defaultValue;
}

Rgba XMLAttribute::Value( const Rgba& defaultValue )
{
    String stringValue = m_attribute.value();

    Rgba color;
    ParseStatus status = color.SetFromText( stringValue );

    if( status == PARSE_SUCCESS )
        return color;
    else
        return defaultValue;
}

Vec2 XMLAttribute::Value( const Vec2& defaultValue )
{
    String stringValue = m_attribute.value();

    Vec2 vec2;
    ParseStatus status = vec2.SetFromText( stringValue );

    if( status == PARSE_SUCCESS )
        return vec2;
    else
        return defaultValue;
}

IRange XMLAttribute::Value( const IRange& defaultValue )
{
    String stringValue = m_attribute.value();

    IRange intRange;
    ParseStatus status = intRange.SetFromText( stringValue );

    if( status == PARSE_SUCCESS )
        return intRange;
    else
        return defaultValue;
}

Range XMLAttribute::Value( const Range& defaultValue )
{
    String stringValue = m_attribute.value();

    Range floatRange;
    ParseStatus status = floatRange.SetFromText( stringValue );

    if( status == PARSE_SUCCESS )
        return floatRange;
    else
        return defaultValue;
}

IVec2 XMLAttribute::Value( const IVec2& defaultValue )
{
    String stringValue = m_attribute.value();

    IVec2 intVec2;
    ParseStatus status = intVec2.SetFromText( stringValue );

    if( status == PARSE_SUCCESS )
        return intVec2;
    else
        return defaultValue;
}

std::vector<int> XMLAttribute::Value( const std::vector<int>& defaultValue )
{
    String stringValue = m_attribute.value();
    std::vector<int> out_ints;
    ParseStatus status = StringUtils::ParseToInts( stringValue, out_ints );

    if( status == PARSE_SUCCESS )
        return out_ints;
    else
        return defaultValue;
}

std::vector<float> XMLAttribute::Value( const std::vector<float>& defaultValue )
{
    String stringValue = m_attribute.value();
    std::vector<float> out_floats;
    ParseStatus status = StringUtils::ParseToFloats( stringValue, out_floats );

    if( status == PARSE_SUCCESS )
        return out_floats;
    else
        return defaultValue;
}


AABB2 XMLAttribute::Value( const AABB2& defaultValue )
{
    String stringValue = m_attribute.value();

    AABB2 aabb2;
    ParseStatus status = aabb2.SetFromText( stringValue );

    if( status == PARSE_SUCCESS )
        return aabb2;
    else
        return defaultValue;
}

XMLAttribute XMLAttribute::NextAttribute() const
{
    return XMLAttribute( m_attribute.next_attribute() );
}

bool XMLAttribute::Valid() const
{
    return m_attribute;
}

XMLAttribute XMLElement::Attribute( const String& attributeName ) const
{
    return XMLAttribute( GetNode().attribute( attributeName.c_str() ) );
}

XMLElement XMLElement::Child( const String& elementName ) const
{
    return XMLElement( GetNode().child( elementName.c_str() ) );
}

XMLElement XMLElement::Parent() const
{
    return XMLElement( GetNode().parent() );
}

XMLAttribute XMLElement::FirstAttribute() const
{
    return XMLAttribute( GetNode().first_attribute() );
}

XMLAttribute XMLElement::LastAttribute() const
{
    return XMLAttribute( GetNode().last_attribute() );
}

XMLElement XMLElement::FirstChild() const
{
    return XMLElement( GetNode().first_child() );
}

XMLElement XMLElement::LastChild() const
{
    return XMLElement( GetNode().last_child() );
}

XMLElement XMLElement::NextSibling() const
{
    return XMLElement( GetNode().next_sibling() );
}

const String XMLElement::Name() const
{
    return GetNode().name();
}

bool XMLElement::Valid() const
{
    return GetNode();
}

void XMLDocument::LoadFromFile( const String& filePath )
{
    pugi::xml_parse_result result = m_document.load_file( filePath.c_str() );

    if( !result )
        LOG_ASSET_LOAD_FAILED( filePath );
}

int ParseXmlAttribute( const XMLElement& element, const char* attributeName, int defaultValue )
{
    return element.Attribute( attributeName ).Value( defaultValue );
}

char ParseXmlAttribute( const XMLElement& element, const char* attributeName, char defaultValue )
{
    return element.Attribute( attributeName ).Value( defaultValue );
}

bool ParseXmlAttribute( const XMLElement& element, const char* attributeName, bool defaultValue )
{
    return element.Attribute( attributeName ).Value( defaultValue );
}

float ParseXmlAttribute( const XMLElement& element, const char* attributeName, float defaultValue )
{
    return element.Attribute( attributeName ).Value( defaultValue );
}

Rgba ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Rgba& defaultValue )
{
    return element.Attribute( attributeName ).Value( defaultValue );
}

Vec2 ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Vec2& defaultValue )
{
    return element.Attribute( attributeName ).Value( defaultValue );
}

IRange ParseXmlAttribute( const XMLElement& element, const char* attributeName, const IRange& defaultValue )
{
    return element.Attribute( attributeName ).Value( defaultValue );
}

Range ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Range& defaultValue )
{
    return element.Attribute( attributeName ).Value( defaultValue );
}

IVec2 ParseXmlAttribute( const XMLElement& element, const char* attributeName, const IVec2& defaultValue )
{
    return element.Attribute( attributeName ).Value( defaultValue );
}

String ParseXmlAttribute( const XMLElement& element, const char* attributeName, const String& defaultValue )
{
    return element.Attribute( attributeName ).Value( defaultValue );
}

std::vector<int> ParseXmlAttribute( const XMLElement& element, const char* attributeName, const std::vector<int>& defaultValue )
{
    return element.Attribute( attributeName ).Value( defaultValue );
}

String ParseXmlAttribute( const XMLElement& element, const char* attributeName, const char* defaultValue )
{
    return element.Attribute( attributeName ).Value( defaultValue );
}

AABB2 ParseXmlAttribute( const XMLElement& element, const char* attributeName, const AABB2& defaultValue )
{
    return element.Attribute( attributeName ).Value( defaultValue );
}

