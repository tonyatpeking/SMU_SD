
#pragma once
#include <string>
#include <vector>

#include "ThirdParty/pugixml/pugixml.hpp"

#include "Engine/Core/EngineCommonH.hpp"


class Rgba;
class Vec2;
class IRange;
class Range;
class IVec2;
class SpriteSheet;
class AABB2;

class XMLAttribute
{
    friend class XMLElement;
public:
    XMLAttribute() {};
    ~XMLAttribute() {};

    String Name() const;

    String Value( const char* defaultValue=nullptr );
    String Value( const String& defaultValue );
    int Value( int defaultValue );
    char Value( char defaultValue );
    bool Value( bool defaultValue );
    float Value( float defaultValue );
    Rgba Value( const Rgba& defaultValue );
    Vec2 Value( const Vec2& defaultValue );
    IRange Value( const IRange& defaultValue );
    Range Value( const Range& defaultValue );
    IVec2 Value( const IVec2& defaultValue );
    AABB2 Value( const AABB2& defaultValue );
    std::vector<int> Value( const std::vector<int>& defaultValue );
    std::vector<float> Value( const std::vector<float>& defaultValue );

    XMLAttribute NextAttribute() const;
    bool Valid() const;

private:
    XMLAttribute( const pugi::xml_attribute& attribute ) { m_attribute = attribute; };
    pugi::xml_attribute m_attribute;
};


class XMLElement
{
public:
    XMLElement() {};
    virtual ~XMLElement() {};
    XMLAttribute Attribute( const String& attributeName ) const;
    XMLElement Child( const String& elementName ) const;
    XMLElement Parent() const;

    XMLAttribute FirstAttribute() const;
    XMLAttribute LastAttribute() const;
    XMLElement FirstChild() const;
    XMLElement LastChild() const;
    XMLElement NextSibling() const;

    const String Name() const;
    bool Valid() const;

protected:
    XMLElement( const pugi::xml_node& node ) { m_node = node; };
    virtual const pugi::xml_node& GetNode() const { return m_node; };
private:
    pugi::xml_node m_node;
};


class XMLDocument : public XMLElement
{
public:
    XMLDocument() {};
    ~XMLDocument() override {};
    void LoadFromFile( const String& filePath );
private:
    const pugi::xml_node& GetNode() const override { return m_document; };
    pugi::xml_document m_document;
};


int ParseXmlAttribute( const XMLElement& element, const char* attributeName, int defaultValue );
char ParseXmlAttribute( const XMLElement& element, const char* attributeName, char defaultValue );
bool ParseXmlAttribute( const XMLElement& element, const char* attributeName, bool defaultValue );
float ParseXmlAttribute( const XMLElement& element, const char* attributeName, float defaultValue );
Rgba ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Rgba& defaultValue );
Vec2 ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Vec2& defaultValue );
IRange ParseXmlAttribute( const XMLElement& element, const char* attributeName, const IRange& defaultValue );
Range ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Range& defaultValue );
IVec2 ParseXmlAttribute( const XMLElement& element, const char* attributeName, const IVec2& defaultValue );
AABB2 ParseXmlAttribute( const XMLElement& element, const char* attributeName, const AABB2& defaultValue );
String ParseXmlAttribute( const XMLElement& element, const char* attributeName, const String& defaultValue );
String ParseXmlAttribute( const XMLElement& element, const char* attributeName, const char* defaultValue );
std::vector<int> ParseXmlAttribute( const XMLElement& element, const char* attributeName, const std::vector<int>& defaultValue );

// this will get the xml attribute with the same name as attrName, minus any m_ g_ s_ prefix
// and assign the value to attrName converted to the correct type
// This macro reduces the chances of typos for attrName as a string and reduces boilerplate code
// attrName must already be declared in the current scope
// e.g. LOAD_XML_ATTRIBUTE( xmlElement, m_name );
//   is equal to { m_name = ParseXmlAttribute(xmlElement, "m_name" , m_name ); }

#define LOAD_XML_ATTRIBUTE( xmlElement, attrName )                                                   \
{                                                                                                    \
    attrName = ParseXmlAttribute(xmlElement, (STRINGIZE_NO_PREFIX( attrName )).c_str(), attrName ); \
}