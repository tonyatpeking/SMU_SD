#pragma once
#include <string>
#include <map>
#include "Engine/Core/EngineCommonH.hpp"


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
    void PopulateFromFile( const string& path );

    void SetValue( const string& keyName, const string& newValue );
    bool GetValue( const string& keyName, bool defaultValue ) const;
    int GetValue( const string& keyName, int defaultValue ) const;
    float GetValue( const string& keyName, float defaultValue ) const;
    char GetValue( const string& keyName, char defaultValue ) const;
    string GetValue( const string& keyName, const string& defaultValue ) const;
    string GetValue( const string& keyName, const char* defaultValue ) const;
    Rgba GetValue( const string& keyName, const Rgba& defaultValue ) const;
    Vec2 GetValue( const string& keyName, const Vec2& defaultValue ) const;
    IVec2 GetValue( const string& keyName, const IVec2& defaultValue ) const;
    Range GetValue( const string& keyName, const Range& defaultValue ) const;
    IRange GetValue( const string& keyName, const IRange& defaultValue ) const;
private:

    map< string, string >	m_keyValuePairs;
};

