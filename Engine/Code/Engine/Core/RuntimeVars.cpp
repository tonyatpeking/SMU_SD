#include "Engine/Core/RuntimeVars.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/String/StringUtils.hpp"

namespace RuntimeVars
{
// <VarName, Value>
map<string, string> g_allVars;


void PopulateFromCommandLine( char* commandline )
{
    string str = string( commandline );
    Strings args;
    StringUtils::ParseToTokens( str, args, " ", true, false );
    for ( auto& arg : args )
    {
        Strings argAndValue;
        StringUtils::ParseToTokens( arg, argAndValue, "=", true, false );
        if( argAndValue.size() == 1 )
        {
            if( StringUtils::IsAllWhitespace( argAndValue[0] ) )
                continue;
            SetVar( arg, true );
        }
        if( argAndValue.size() == 2 )
        {
            if( StringUtils::IsAllWhitespace( argAndValue[0] ) )
                continue;
            SetVar( argAndValue[0], argAndValue[1] );
        }
    }
}

void LogAll()
{
    LOG_INFO_TAG( "Vars", "Printing all Runtime Vars:" )
    for ( auto& pair : g_allVars )
    {
        LOG_INFO_TAG( "Vars", "    %s : %s", pair.first.c_str(), pair.second.c_str() );
    }
}

bool HasVar( string var )
{
    return ContainerUtils::ContainsKey( g_allVars, var );
}

bool EraseVar( string var )
{
    return 0 != g_allVars.erase( var );
}

bool IsBoolSet( string var )
{
    bool b;
    GetVar( var, b, false );
    return b;
}

}