#pragma once
#include <string>
#include <map>
#include "Engine/Core/EngineCommon.hpp"


class XMLElement;
class Rgba;
class Vec2;
class IVec2;
class Range;
class IRange;

class Blackboard
{
public:
    void PopulateFromXmlElementAttributes( const XMLElement& element );
    void PopulateFromFile( const String& path );

    void SetValue( const String& keyName, const String& newValue );
    bool GetValue( const String& keyName, bool defaultValue ) const;
    int GetValue( const String& keyName, int defaultValue ) const;
    float GetValue( const String& keyName, float defaultValue ) const;
    char GetValue( const String& keyName, char defaultValue ) const;
    String GetValue( const String& keyName, const String& defaultValue ) const;
    String GetValue( const String& keyName, const char* defaultValue ) const;
    Rgba GetValue( const String& keyName, const Rgba& defaultValue ) const;
    Vec2 GetValue( const String& keyName, const Vec2& defaultValue ) const;
    IVec2 GetValue( const String& keyName, const IVec2& defaultValue ) const;
    Range GetValue( const String& keyName, const Range& defaultValue ) const;
    IRange GetValue( const String& keyName, const IRange& defaultValue ) const;
private:

    std::map< String, String >	m_keyValuePairs;
};

