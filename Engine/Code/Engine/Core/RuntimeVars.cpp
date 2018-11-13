#include "Engine/Core/RuntimeVars.hpp"
#include "Engine/Core/EngineCommonC.hpp"

namespace RuntimeVars
{
// <VarName, Value>
map<string, string> g_allVars;


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