#pragma once
#include "Engine/Core/EngineCommonH.hpp"

namespace RuntimeVars
{

extern map<string, string> g_allVars;


void LogAll();

bool HasVar( string var );

// returns if anything was erased
bool EraseVar( string var );

// treats unset bool as false
bool IsBoolSet( string var );


template<typename T>
void SetVar( string var, const T& value )
{
    g_allVars[var] = ToString( value );
}

template<typename T>
bool GetVar( string var, T& out_value )
{
    if( HasVar( var ) )
    {
        SetFromString( g_allVars[var], out_value );
        return true;
    }
    return false;
}

template<typename T>
bool GetVar( string var, T& out_value, const T& defaultValue )
{
    if( HasVar( var ) )
    {
        SetFromString( g_allVars[var], out_value );
        return true;
    }
    out_value = defaultValue;
    return false;
}

};
