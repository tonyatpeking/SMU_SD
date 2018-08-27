#include <stdarg.h>
#include <algorithm>

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Range.hpp"
#include "Engine/Math/IRange.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Mat4.hpp"


namespace StringUtils
{

std::vector<char> s_whitespace ={ ' ', '\n', '\t', '\r' };
std::vector<char> s_openingBrackets ={ '(','[','{','<','\'','\"' };
std::vector<char> s_closingBrackets ={ ')',']','}','>','\'','\"' };
std::map<char, char> s_matchingBrackets ={ { '(',')' },{ '[',']' },{ '{','}' },
                                         { '<','>' },{ '\"','\"' },{'\'', '\'' },
                                         { ')','(' },{ ']','[' },{ '}','{' },
                                         { '>','<' } };

const int Stringf_STACK_LOCAL_TEMP_LENGTH = 2048;


ParseStatus ParseToInts( const String& str, std::vector<int>& out_ints,
                         const String& delimiter /*= "," */ )
{
    out_ints.clear();
    ParseStatus status = PARSE_SUCCESS;

    if( str == "" )
    {
        status = PARSE_EMPTY;
    }

    Strings out_tokens;

    ParseToTokens( str, out_tokens, delimiter );

    for( size_t tokenIdx = 0; tokenIdx < out_tokens.size(); ++tokenIdx )
    {
        int out_int = 0;
        String& token = out_tokens[tokenIdx];
        if( SetFromString( token, out_int ) != PARSE_SUCCESS
            && status == PARSE_SUCCESS )
        {
            status = PARSE_INVALID_TOKEN;
        }
        out_ints.push_back( out_int );
    }

    return status;
}


ParseStatus ParseToFloats( const String& str, std::vector<float>& out_floats,
                           const String& delimiter /*= "," */ )
{
    out_floats.clear();
    ParseStatus status = PARSE_SUCCESS;

    if( str == "" )
    {
        status = PARSE_EMPTY;
    }

    Strings out_tokens;

    ParseToTokens( str, out_tokens, delimiter );

    for( size_t tokenIdx = 0; tokenIdx < out_tokens.size(); ++tokenIdx )
    {
        float out_float = 0;
        String& token = out_tokens[tokenIdx];
        if( SetFromString( token, out_float ) != PARSE_SUCCESS
            && status == PARSE_SUCCESS )
        {
            status = PARSE_INVALID_TOKEN;
        }
        out_floats.push_back( out_float );
    }

    return status;
}

ParseStatus ParseToBools( const String& str, std::vector<bool>& out_bools,
                          const String& delimiter /*= "," */ )
{
    out_bools.clear();
    ParseStatus status = PARSE_SUCCESS;

    if( str == "" )
    {
        status = PARSE_EMPTY;
    }

    Strings out_tokens;

    ParseToTokens( str, out_tokens, delimiter );

    for( size_t tokenIdx = 0; tokenIdx < out_tokens.size(); ++tokenIdx )
    {
        bool out_bool = 0;
        String& token = out_tokens[tokenIdx];
        if( SetFromString( token, out_bool ) != PARSE_SUCCESS
            && status == PARSE_SUCCESS )
        {
            status = PARSE_INVALID_TOKEN;
        }
        out_bools.push_back( out_bool );
    }

    return status;
}

ParseStatus ParseToTokens( const String& str, Strings& out_tokens,
                           const String& delimiter, bool removeWhiteSpace,
                           bool removeBrackets, bool removeDelimiter )
{
    out_tokens.clear();
    size_t pos = 0;
    String token;
    String strCopy = str;
    size_t delimiterLenToCopy = 0;
    if( delimiter == " " )
        removeWhiteSpace = false;
    if( !removeDelimiter )
        delimiterLenToCopy = delimiter.length();
    if( removeWhiteSpace )
        RemoveWhitespace( strCopy );
    if( removeBrackets )
        RemoveStartAndEndBrackets( strCopy );
    while( ( pos = strCopy.find( delimiter ) ) != String::npos )
    {
        token = strCopy.substr( 0, pos + delimiterLenToCopy );

        if( removeWhiteSpace )
            RemoveWhitespace( token );


        out_tokens.push_back( token );
        strCopy.erase( 0, pos + delimiter.length() );
    }

    if( removeWhiteSpace )
        RemoveWhitespace( strCopy );

    //note: last token still needs to be pushed back outside loop
    out_tokens.push_back( strCopy );
    return PARSE_SUCCESS;
}



ParseStatus ParseToLines( const String& str,
                          Strings& out_tokens,
                          bool removeNewline, /*= true */
                          bool addNewlineToEnd  /*= false */ )
{
    ParseStatus status = ParseToTokens( str, out_tokens, "\n", false,
                                        false, removeNewline );
    if( addNewlineToEnd )
    {
        if( out_tokens.back().empty() || out_tokens.back().back() != '\n' )
            out_tokens.back().push_back( '\n' );
    }

    return status;
}

ParseStatus ParseParameters( const String& str,
                             Strings& out_parameters )
{
    // Will parse "function_name(a,b,c,(1,2),[3,4],{5,6},"[]([  ](]]")" into
    // "a" "b" "c" "1,2" "3,4" "5,6" "[]([  ](]]"
    // Will parse "add 2 3" to "2" "3"
    // Note: will not work if there are nested brackets e.g. [[2,3]]
    out_parameters.clear();
    String paramsStr;
    ExtractOnlyParameters( str, paramsStr );

    char delimiter = ',';
    // the next char we are looking for to end a parameter
    char closingBracket = '\0';
    size_t startPos = 0;
    size_t currentPos = 0;
    enum ParseState
    {
        //triggered after a delimiter
        AFTER_DELIMITER,
        //currentPos is on a parameter
        ON_PARAM,
        //currentPos is after a parameter (triggered after a whitespace)
        WHITESPACE_AFTER_PARAM
    };
    enum CharCategory
    {
        //anything that is not a space or delimiter
        PARAM_CHAR,
        DELIMITER,
        WHITESPACE
    };
    // set it to this state so the first delimiter will push_back an empty parameter
    ParseState currentState = AFTER_DELIMITER;
    CharCategory charCategory;

    while( currentPos < paramsStr.size() )
    {
        char currentChar = paramsStr[currentPos];
        if( IsWhitespace( currentChar ) )
            charCategory = WHITESPACE;
        else if( currentChar == delimiter )
            charCategory = DELIMITER;
        else
            charCategory = PARAM_CHAR;

        // we were not on a parameter and we step onto one
        if( currentState != ON_PARAM && charCategory == PARAM_CHAR )
        {
            currentState = ON_PARAM;
            startPos = currentPos;
            if( IsCharInList( currentChar, s_openingBrackets ) )
            {
                closingBracket = GetMatchingBracket( currentChar );
            }
        }
        // not on a parameter, skip whitespace
        else if( currentState != ON_PARAM && charCategory == WHITESPACE )
        {
            ;
        }
        // after a delimiter if another is encountered then push an empty parameter
        else if( currentState == AFTER_DELIMITER && charCategory == DELIMITER )
        {
            out_parameters.push_back( "" );
        }
        // after whitespace
        else if( currentState == WHITESPACE_AFTER_PARAM && charCategory == DELIMITER )
        {
            currentState = AFTER_DELIMITER;
        }
        // on a parameter waiting for a closing bracket,
        // and stepped onto a closing bracket
        else if( currentState == ON_PARAM && currentChar == closingBracket )
        {
            closingBracket = '\0';
        }
        // on a parameter, all brackets closed, and stepped onto a opening bracket
        else if( currentState == ON_PARAM && closingBracket == '\0'
                 && IsCharInList( currentChar, s_openingBrackets ) )
        {
            closingBracket = GetMatchingBracket( currentChar );
        }
        // on a parameter, stepped onto delimiter
        else if( currentState == ON_PARAM && closingBracket == '\0'
                 && charCategory == DELIMITER )
        {
            size_t length = currentPos - startPos;
            out_parameters.push_back( paramsStr.substr( startPos, length ) );
            currentState = AFTER_DELIMITER;
        }
        // on a parameter, stepped onto whitespace
        else if( currentState == ON_PARAM && closingBracket == '\0'
                 && charCategory == WHITESPACE )
        {
            size_t length = currentPos - startPos;
            out_parameters.push_back( paramsStr.substr( startPos, length ) );
            currentState = WHITESPACE_AFTER_PARAM;
        }

        ++currentPos;
    }

    if( closingBracket != '\0' ) //no matching ending bracket found
        return PARSE_INVALID_INPUT;

    if( currentState == ON_PARAM ) //push the last param
        out_parameters.push_back( paramsStr.substr( startPos ) );

    return PARSE_SUCCESS;
}

ParseStatus ExtractOnlyParameters( const String& str, String& out_parameters )
{
    // Remove everything not inside the outermost "( )"
    // Note there is a space in there
    size_t parametersStartPos = str.find_first_of( "( " );
    // no parameters, in this case "some_function"
    if( parametersStartPos == String::npos )
        return PARSE_SUCCESS;

    size_t parametersEndPos;
    if( str[parametersStartPos] == '(' )
        parametersEndPos = str.rfind( ')' );
    // this is actually 1 after the end,
    // we use this to stay consistent with the ')' that is sometimes at the end
    else
        parametersEndPos = str.size();


    if( parametersEndPos == String::npos ) // there is no matching ending bracket
        return PARSE_INVALID_INPUT;

    int parametersLength = (int) parametersEndPos - (int) parametersStartPos - 1;
    if( parametersLength < 0 )
        return PARSE_INVALID_INPUT;
    if( parametersLength == 0 ) // no parameters, in this case "some_function()"
        return PARSE_SUCCESS;
    out_parameters = str.substr( parametersStartPos + 1, parametersLength );
    return PARSE_SUCCESS;
}

ParseStatus ParseFunctionName( const String& str, String& out_functionName )
{
    //Note there is a space in there
    size_t parametersStartPos = str.find_first_of( "( " );
    if( parametersStartPos == 0 )
        return PARSE_EMPTY;
    out_functionName = str.substr( 0, parametersStartPos );
    return PARSE_SUCCESS;
}


ParseStatus ParseEnumStringToMap( const String& str, std::map<int, String>& stringMap )
{
    ParseStatus parseStatus = PARSE_SUCCESS;
    Strings enumEntrys;
    ParseToTokens( str, enumEntrys );  // An entry could be "ONE = 1" or "TWO"

    stringMap.clear();
    int enumIntVal = 0;
    for( auto& enumEntry : enumEntrys )
    {
        Strings tokens; // A token would be "ONE", "1"
        ParseToTokens( enumEntry, tokens, "=" );

        if( tokens.size() == 0 || tokens.size() > 2 )
            parseStatus = ParseStatus::PARSE_WRONG_NUMBER_OF_TOKENS;

        // Set the enumIntVal if there is a "="
        else if( tokens.size() == 2 )
            parseStatus = SetFromString( tokens[1], enumIntVal );

        // Duplicate enum int
        if( stringMap.find( enumIntVal ) != stringMap.end() )
            parseStatus = ParseStatus::PARSE_INVALID_TOKEN;

        stringMap[enumIntVal] = tokens[0];
        enumIntVal++;

    }
    return parseStatus;
}



String ConcatStrings( const Strings& strings, bool newlineAfterEachLine )
{
    size_t totalSize = 0;
    for( auto& str : strings )
        totalSize += str.size();

    if( newlineAfterEachLine )
        totalSize += strings.size();

    String concatStr;
    concatStr.reserve( totalSize );
    for( auto& str : strings )
    {
        concatStr += str;
        if( newlineAfterEachLine )
            concatStr += '\n';
    }

    return concatStr;
}

void ReplaceAll( String& out_str, char toBeReplaced, char replaceWith )
{
    size_t strLen = out_str.size();
    for( size_t strIdx = 0; strIdx < strLen; ++strIdx )
    {
        if( out_str[strIdx] == toBeReplaced )
            out_str[strIdx] = replaceWith;
    }
}

bool IsWhitespace( char charToTest )
{
    return IsCharInList( charToTest, s_whitespace );
}

bool IsAllWhitespace( String& strToTest )
{
    for( char c : strToTest )
    {
        if( !IsWhitespace( c ) )
            return false;
    }
    return true;
}

bool FindInStrings( const Strings& strings, const String strToFind,
                    size_t& out_stringIdx, size_t& out_pos )
{
    size_t pos;
    for( size_t strIdx = 0; strIdx < strings.size(); ++strIdx )
    {
        pos = strings[strIdx].find( strToFind );
        if( pos != String::npos )
        {
            out_stringIdx = strIdx;
            out_pos = pos;
            return true;
        }
    }
    return false;
}

bool IsCharInList( char charToTest, const std::vector<char>& charList )
{
    for( const auto charInList : charList )
    {
        if( charToTest == charInList )
            return true;
    }
    return false;
}

void RemoveWhitespace( String& out_str )
{
    for( const auto whiteSpaceChar : s_whitespace )
    {
        RemoveAllChar( out_str, whiteSpaceChar );
    }
}

void RemoveOuterWhitespace( String& out_str )
{
    // start from back and move forward
    for( int strIdx = (int) out_str.size() - 1; strIdx >= 0; --strIdx )
    {
        if( IsWhitespace( out_str[strIdx] ) )
            out_str.pop_back();
        else
            break;
    }

    if( out_str.empty() )
        return;

    String strCopy;
    size_t firstNonWhite = out_str.size();
    // start from front and move back
    for( size_t strIdx = 0; strIdx < out_str.size(); ++strIdx )
    {
        char c = out_str[strIdx];
        if( IsWhitespace( c ) )
            continue;
        firstNonWhite = strIdx;
        break;
    }

    strCopy.append( out_str, firstNonWhite, String::npos );
    out_str = strCopy;
}

void RemoveAllChar( String& out_str, const char& charToRemove )
{
    String::iterator end_pos
        = std::remove( out_str.begin(), out_str.end(), charToRemove );

    out_str.erase( end_pos, out_str.end() );
}

void RemoveStartAndEndBrackets( String& out_str )
{
    if( out_str.empty() )
        return;
    for( char openingBracket : s_openingBrackets )
    {
        if( *out_str.begin() == openingBracket )
        {
            out_str.erase( out_str.begin() );
            break;
        }
    }
    for( char closingBracket : s_closingBrackets )
    {
        if( *( out_str.end() - 1 ) == closingBracket )
        {
            out_str.erase( out_str.end() - 1 );
            break;
        }
    }
}

bool ContainsSubstring( const String& str, const String& subStr )
{
    return str.find( subStr ) != String::npos;
}

void ChopStringWithMaxLength( const String& str, Strings& out_substrings, int maxLength )
{
    out_substrings.clear();

    if( str.empty() )
    {
        out_substrings.push_back( "" );
        return;
    }

    int strLen = (int) str.size();
    for( int substringStart = 0; substringStart < strLen; substringStart += maxLength )
    {
        out_substrings.push_back( str.substr( substringStart, maxLength ) );
    }
}

void ToLower( String& out_str )
{
    for( char& c : out_str )
    {
        c = (char) tolower( c );
    }
}

void ToUpper( String& out_str )
{
    for( char& c : out_str )
    {
        c = (char) toupper( c );
    }
}

char GetMatchingBracket( char bracket )
{
    return s_matchingBrackets[bracket];
}


void ReplaceComments( String& str, char replaceWith /*= '*' */ )
{
    size_t pos = 0;
    //replace "/* words */"
    while( true )
    {
        pos = str.find( "/*", pos );
        if( String::npos == pos )
            break;
        size_t endPos = str.find( "*/", pos + 2 ); // + 2 because /*/ is not valid end
        if( String::npos == endPos )
        {
            LOG_WARNING( "No matching */ found to end comment" );
            break;
        }
        /*replace*/
        /*********/
        size_t len = endPos - pos - 2;
        str.replace( pos + 2, len, len, replaceWith );
        pos = endPos + 2;
    }

    pos = 0;
    //replace "// words /n"
    while( true )
    {
        pos = str.find( "//", pos );
        if( String::npos == pos )
            break;
        size_t endPos = str.find( '\n', pos + 2 ); // + 2 because /*/ is not valid end
        if( String::npos == endPos )
            endPos = str.size(); //123/n
        size_t len = endPos - pos - 2;
        str.replace( pos + 2, len, len, replaceWith );
        pos = endPos + 1;
    }
}

const String RemovePrefix( const String& str )
{
    if( str.size() < 3 )
        return str;

    if( str[1] == '_' )
    {
        if( str[0] == 'm' || str[0] == 's' || str[0] == 'g' )
        {
            return str.substr( 2 );
        }
    }
    return str;
}



}



const String Stringf( const char* format, ... )
{
    char textLiteral[StringUtils::Stringf_STACK_LOCAL_TEMP_LENGTH];
    va_list variableArgumentList;
    va_start( variableArgumentList, format );
    vsnprintf_s( textLiteral, StringUtils::Stringf_STACK_LOCAL_TEMP_LENGTH,
                 _TRUNCATE, format, variableArgumentList );
    va_end( variableArgumentList );
    // In case vsnprintf overran (doesn't auto-terminate)
    textLiteral[StringUtils::Stringf_STACK_LOCAL_TEMP_LENGTH - 1] = '\0';

    return String( textLiteral );
}

const String Stringf( const char* format, va_list args )
{
    char textLiteral[StringUtils::Stringf_STACK_LOCAL_TEMP_LENGTH];
    vsnprintf_s( textLiteral, StringUtils::Stringf_STACK_LOCAL_TEMP_LENGTH,
                 _TRUNCATE, format, args );
    // In case vsnprintf overran (doesn't auto-terminate)
    textLiteral[StringUtils::Stringf_STACK_LOCAL_TEMP_LENGTH - 1] = '\0';
    return String( textLiteral );
}

const String Stringf( const int maxLength, const char* format, ... )
{
    char textLiteralSmall[StringUtils::Stringf_STACK_LOCAL_TEMP_LENGTH];
    char* textLiteral = textLiteralSmall;
    if( maxLength > StringUtils::Stringf_STACK_LOCAL_TEMP_LENGTH )
        textLiteral = new char[maxLength];

    va_list variableArgumentList;
    va_start( variableArgumentList, format );
    vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );
    va_end( variableArgumentList );
    // In case vsnprintf overran (doesn't auto-terminate)
    textLiteral[maxLength - 1] = '\0';

    String returnValue( textLiteral );
    if( maxLength > StringUtils::Stringf_STACK_LOCAL_TEMP_LENGTH )
        delete[] textLiteral;

    return returnValue;
}

ParseStatus SetFromString( const String& str, int& out_int )
{
    out_int = atoi( str.c_str() );

    //atoi returns 0 on non numbers, so check if str is non char
    if( out_int == 0 )
    {
        if( str.empty() )
            return PARSE_EMPTY;

        String out_strCopy = str;

        if( out_strCopy[0] == '0' )
            return PARSE_SUCCESS;
        else
            return PARSE_INVALID_TOKEN;
    }

    return PARSE_SUCCESS;
}

ParseStatus SetFromString( const String& str, float& out_float )
{
    out_float = (float) atof( str.c_str() );

    //atof returns 0.f on non numbers, so check if str is non char
    if( out_float == 0.f )
    {
        if( str.empty() )
            return PARSE_EMPTY;

        String out_strCopy = str;

        if( out_strCopy[0] == '0' || out_strCopy[0] == '-' )
            return PARSE_SUCCESS;
        else
            return PARSE_INVALID_TOKEN;
    }

    return PARSE_SUCCESS;
}

ParseStatus SetFromString( const String& str, bool& out_bool )
{

    String out_strCopy = str;
    if( out_strCopy == "true" || out_strCopy == "True" )
    {
        out_bool = true;
        return PARSE_SUCCESS;
    }
    else if( out_strCopy == "false" || out_strCopy == "False" )
    {
        out_bool = false;
        return PARSE_SUCCESS;
    }
    else
    {
        int out_int;
        ParseStatus status = SetFromString( str, out_int );
        out_bool = ( out_int == 1 );
        return status;
    }
}
ParseStatus SetFromString( const String& str, unsigned char& out_var )
{
    int out_int = 0;
    ParseStatus status = SetFromString( str, out_int );
    out_var = (unsigned char) out_int;
    return status;
}

ParseStatus SetFromString( const String& str, String& out_var )
{
    out_var = str;
    return PARSE_SUCCESS;
}



ParseStatus SetFromString( const String& str, uint& out_int )
{
    int i;
    ParseStatus status = SetFromString( str, i );
    out_int = (uint) i;
    return status;
}

const String ToString( int var )
{
    return std::to_string( var );
}

const String ToString( float var )
{
    return Stringf( "%.2f", var );
}

const String ToString( bool var )
{
    if( var )
        return "true";

    return "false";
}

const String ToString( unsigned char var )
{
    return std::to_string( var );
}

const String ToString( const Rgba& var )
{
    return Stringf( "Rgba(%d, %d, %d, %d)", var.r, var.g, var.b, var.a );
}

const String ToString( const AABB2& var )
{
    return Stringf( "AABB2(%.2f, %.2f, %.2f, %.2f)", var.mins.x, var.mins.y,
                    var.maxs.x, var.maxs.y );
}

const String ToString( const Range& var )
{
    return Stringf( "Range(%.2f, %.2f)", var.min, var.max );
}

const String ToString( const IRange& var )
{
    return Stringf( "IRange(%d, %d)", var.min, var.max );
}

const String ToString( const IVec2& var )
{
    return Stringf( "IVec2(%d, %d)", var.x, var.y );
}

const String ToString( const Vec2& var )
{
    return Stringf( "Vec2(%.2f, %.2f)", var.x, var.y );
}

const String ToString( const String& var )
{
    return var;
}

const String ToString( const Vec3& var )
{
    return Stringf( "Vec3(%.2f, %.2f, %.2f)", var.x, var.y, var.z );
}

const String ToString( const Vec4& var )
{
    return Stringf( "Vec4(%.2f, %.2f, %.2f, %.2f)", var.x, var.y, var.z, var.w );
}

const String ToString( const Mat4& var )
{
    return Stringf( "%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f",
                    var.Ix, var.Jx, var.Kx, var.Tx,
                    var.Iy, var.Jy, var.Ky, var.Ty,
                    var.Iz, var.Jz, var.Kz, var.Tz,
                    var.Iw, var.Jw, var.Kw, var.Tw
    );
}

const String ToString( uint var )
{
    return ToString( (int) var );
}
