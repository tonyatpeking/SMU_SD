#pragma once

#include <map>
#include "Engine/Core/Types.hpp"
#include "Engine/Core/ParseStatus.hpp"

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

extern std::vector<char> s_whitespace;
extern std::vector<char> s_openingBrackets;
extern std::vector<char> s_closingBrackets;
extern std::map<char, char> s_matchingBrackets;

// Parsing with delimiters
ParseStatus ParseToInts( const String& str, std::vector<int>& out_ints,
                         const String& delimiter = "," );

ParseStatus ParseToFloats( const String& str, std::vector<float>& out_floats,
                           const String& delimiter = "," );

ParseStatus ParseToBools( const String& str, std::vector<bool>& out_bools,
                          const String& delimiter = "," );
// Token parsing
// Tokens are the substrings that are separated by the delimiter,
// e.g. for "a,b,c" tokens are "a" "b" "c", white space is not stripped
ParseStatus ParseToTokens( const String& str, Strings& out_tokens,
                           const String& delimiter = ",", bool removeWhiteSpace = true,
                           bool removeBrackets = true, bool removeDelimiter = true );

ParseStatus ParseToLines( const String& str, Strings& out_tokens,
                          bool removeNewline = true, bool addNewlineToEnd = false );

// Command/function parsing
// Will parse "function_name(a,b,c,(1,2),[3,4],{5,6},"[]([  ](]]")"
// into "a" "b" "c" "1,2" "3,4" "5,6" "[]([  ](]]"
// Will parse "add 2 3" to "2" "3"
// Note: will not work if there are nested brackets e.g. [[2,3]]
ParseStatus ParseParameters( const String& str,
                             Strings& out_parameters );
// Will parse "function_name(a,b)" to "a,b"   or   "fun a b" to "a b"
ParseStatus ExtractOnlyParameters( const String& str, String& out_parameters );
// Will parse "function_name(a,b,c)" to "function_name"
ParseStatus ParseFunctionName( const String& str, String& out_functionName );

// Enum parsing
// Parses "ONE = 1, TWO, TEN = 10" to
// map<int, string> { { 1, "ONE" }, { 2, "TWO" }, { 10, "TEN" } }
ParseStatus ParseEnumStringToMap( const String& str,
                                  std::map<int, String>& stringMap );

// Utilities
// White space
String ConcatStrings( const Strings& strings, bool newlineAfterEachLine = false );
bool IsWhitespace( char charToTest );
bool IsCharInList( char charToTest, const std::vector<char>& charList );
bool IsAllWhitespace( String& strToTest );

// essentially calls find in each string until strToFind is found
bool FindInStrings( const Strings& strings, const String strToFind,
                    size_t& out_stringIdx, size_t& out_pos );

void RemoveWhitespace( String& out_str );
void RemoveOuterWhitespace( String& out_str );
void RemoveAllChar( String& out_str, const char& charToRemove );
void RemoveStartAndEndBrackets( String& out_str );

void ReplaceAll( String& out_str, char toBeReplaced, char replaceWith );

bool ContainsSubstring( const String& str, const String& subStr );

void ChopStringWithMaxLength( const String& str, Strings& out_substrings,
                              int maxLength );

void ToLower( String& out_str );
void ToUpper( String& out_str );

char GetMatchingBracket( char bracket );

//note does not worked correctly if /* */ or // are in a string literal
void ReplaceComments( String& str, char replaceWith = '*' );

// Removes m_ s_ g_ prefixes from string
const String RemovePrefix( const String& str );

}

// Outside namespace


ParseStatus SetFromString( const String& str, int& out_int );
ParseStatus SetFromString( const String& str, uint& out_int );
ParseStatus SetFromString( const String& str, float& out_float );
ParseStatus SetFromString( const String& str, bool& out_bool );
// Set from string
template <typename T>
ParseStatus SetFromString( const String& str, T& out_var, const String& delimiter = "," )
{
    return out_var.SetFromText( str, delimiter );
}

ParseStatus SetFromString( const String& str, unsigned char& out_var );
ParseStatus SetFromString( const String& str, String& out_var );

const String Stringf( const char* format, ... );
const String Stringf( const char* format, va_list args );
const String Stringf( const int maxLength, const char* format, ... );

// Convert to string
const String ToString( int var );
const String ToString( uint var );
const String ToString( float var );
const String ToString( bool var );
const String ToString( unsigned char var );
const String ToString( const Rgba& var );
const String ToString( const AABB2& var );
const String ToString( const Range& var );
const String ToString( const IRange& var );
const String ToString( const IVec2& var );
const String ToString( const Vec2& var );
const String ToString( const Vec3& var );
const String ToString( const Vec4& var );
const String ToString( const Mat4& var );
const String ToString( const String& var );

// Converts someVar to "someVar"
#define STRINGIZE( var ) String( #var )

// Converts m_someVar to "someVar"
#define STRINGIZE_NO_PREFIX( var ) (StringUtils::RemovePrefix( #var ))

// Converts m_someVar to "someVar: varValue"
#define STRINGIZE_FORMAT( var ) (STRINGIZE_NO_PREFIX( var ) + ": " + ::ToString(var))


