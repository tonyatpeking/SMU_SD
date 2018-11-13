#pragma once

#include <map>
#include "Engine/Core/Types.hpp"
#include "Engine/String/ParseStatus.hpp"

class Rgba;
class AABB2;
class Range;
class IRange;
class IVec2;
class Vec2;
class Vec3;
class Vec4;
class Mat4;

namespace StringUtils
{

extern vector<char> s_whitespace;
extern vector<char> s_openingBrackets;
extern vector<char> s_closingBrackets;
extern map<char, char> s_matchingBrackets;

// Parsing with delimiters
ParseStatus ParseToInts( const string& str, vector<int>& out_ints,
                         const string& delimiter = "," );

ParseStatus ParseToFloats( const string& str, vector<float>& out_floats,
                           const string& delimiter = "," );

ParseStatus ParseToBools( const string& str, vector<bool>& out_bools,
                          const string& delimiter = "," );
// Token parsing
// Tokens are the substrings that are separated by the delimiter,
// e.g. for "a,b,c" tokens are "a" "b" "c", white space is not stripped
ParseStatus ParseToTokens( const string& str, Strings& out_tokens,
                           const string& delimiter = ",", bool removeWhiteSpace = true,
                           bool removeBrackets = true, bool removeDelimiter = true );

ParseStatus ParseToLines( const string& str, Strings& out_tokens,
                          bool removeNewline = true, bool addNewlineToEnd = false );

// Command/function parsing
// Will parse "function_name(a,b,c,(1,2),[3,4],{5,6},"[]([  ](]]")"
// into "a" "b" "c" "1,2" "3,4" "5,6" "[]([  ](]]"
// Will parse "add 2 3" to "2" "3"
// Note: will not work if there are nested brackets e.g. [[2,3]]
ParseStatus ParseParameters( const string& str,
                             Strings& out_parameters );
// Will parse "function_name(a,b)" to "a,b"   or   "fun a b" to "a b"
ParseStatus ExtractOnlyParameters( const string& str, string& out_parameters );
// Will parse "function_name(a,b,c)" to "function_name"
ParseStatus ParseFunctionName( const string& str, string& out_functionName );

// Enum parsing
// Parses "ONE = 1, TWO, TEN = 10" to
// map<int, string> { { 1, "ONE" }, { 2, "TWO" }, { 10, "TEN" } }
ParseStatus ParseEnumStringToMap( const string& str,
                                  map<int, string>& stringMap );

// Utilities
// White space
string ConcatStrings( const Strings& strings, bool newlineAfterEachLine = false );
bool IsWhitespace( char charToTest );
bool IsCharInList( char charToTest, const vector<char>& charList );
bool IsAllWhitespace( string& strToTest );
bool IsPositiveInt( string& strToTest );

// essentially calls find in each string until strToFind is found
bool FindInStrings( const Strings& strings, const string strToFind,
                    size_t& out_stringIdx, size_t& out_pos );

void RemoveWhitespace( string& out_str );
void RemoveOuterWhitespace( string& out_str );
void RemoveAllChar( string& out_str, const char& charToRemove );
void RemoveStartAndEndBrackets( string& out_str );

void ReplaceAll( string& out_str, char toBeReplaced, char replaceWith );

bool ContainsSubstring( const string& str, const string& subStr );

void ChopStringWithMaxLength( const string& str, Strings& out_substrings,
                              int maxLength );

void ToLower( string& out_str );
void ToUpper( string& out_str );

char GetMatchingBracket( char bracket );

//note does not worked correctly if /* */ or // are in a string literal
void ReplaceComments( string& str, char replaceWith = '*' );

// Removes m_ s_ g_ prefixes from string
const string RemovePrefix( const string& str );

template <typename T>
const string ToBitfield( T num )
{
    string str;
    size_t bitCount = sizeof( T ) * 8;
    str.resize( bitCount, '0' );
    T mask = (T) 1;
    for( int idx = (int)bitCount - 1; idx >= 0; --idx )
    {
        if( num & mask )
            str[idx] = '1';
        mask <<= 1;
    }
    return str;
}

// formats any type to bytes e.g. 0D FF
template <typename T>
const string ToHex( T num )
{
    return ToHex( &num, sizeof( T ) );
}

const string ToHex( void* buffer, size_t byteCount, bool reverse = false );


}

// Outside namespace


ParseStatus SetFromString( const string& str, int& out_int );
ParseStatus SetFromString( const string& str, uint& out_int );
ParseStatus SetFromString( const string& str, float& out_float );
ParseStatus SetFromString( const string& str, bool& out_bool );
// Set from string
template <typename T>
ParseStatus SetFromString( const string& str, T& out_var, const string& delimiter = "," )
{
    return out_var.SetFromText( str, delimiter );
}

ParseStatus SetFromString( const string& str, unsigned char& out_var );
ParseStatus SetFromString( const string& str, string& out_var );

const string Stringf( const char* format, ... );
const string Stringf( const char* format, va_list args );
const string Stringf( const int maxLength, const char* format, ... );

// Convert to string
const string ToString( int var );
const string ToString( uint var );
const string ToString( float var );
const string ToString( float var, int significantDigits );
const string ToString( bool var );
const string ToString( unsigned char var );
const string ToString( const Rgba& var );
const string ToString( const AABB2& var );
const string ToString( const Range& var );
const string ToString( const IRange& var );
const string ToString( const IVec2& var );
const string ToString( const Vec2& var );
const string ToString( const Vec3& var );
const string ToString( const Vec4& var );
const string ToString( const Mat4& var );
const string ToString( const string& var );

// Converts someVar to "someVar"
#define STRINGIZE( var ) string( #var )

// Converts m_someVar to "someVar"
#define STRINGIZE_NO_PREFIX( var ) (StringUtils::RemovePrefix( #var ))

// Converts m_someVar to "someVar: varValue"
#define STRINGIZE_FORMAT( var ) (STRINGIZE_NO_PREFIX( var ) + ": " + ::ToString(var))


