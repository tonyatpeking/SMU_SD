#pragma once
#include <string>
#include <vector>
#include <map>
#include "Engine/String/StringUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/EngineCommonH.hpp"



typedef void( *CommandCallback )( string& );

// make a static variable of this to self register to the default command system
class CommandSelfRegister
{
    CommandSelfRegister( string& name, CommandCallback callback );
    ~CommandSelfRegister() {};
};

struct CommandDef
{
public:

    CommandDef() {}

    CommandDef( string& name, CommandCallback callback )
        : m_name( name )
        , m_callback( callback )
    {};

    string m_name;

    CommandCallback m_callback = nullptr;
};

class CommandParameterParser
{
public:

    CommandParameterParser( const string& commandString, bool suppressWarnings = false );

    // gets the next param, works for most engine types, return whether succeeded
    template<typename T>
    bool GetNext( T& var )
    {
        if( m_nextParamToGet < m_parameterTokens.size() )
        {
            ParseStatus parseStatus = SetFromString(
                m_parameterTokens[m_nextParamToGet], var );
            ++m_nextParamToGet;
            if( PARSE_SUCCESS == parseStatus )
            {
                return true;
            }
        }

        if( m_allParseSuccess )
        {
            if( !m_suppressWarnings )
            m_allParseSuccess = false;
        }
        return false;
    };


    bool AllParseSuccess() { return m_allParseSuccess; };
    size_t NumOfParams() { return m_parameterTokens.size(); };
    string GetName() { return m_commandName; };
    string GetOnlyParameters() { return m_justParams; };
private:
    Strings m_parameterTokens;
    string m_commandName;
    string m_justParams;
    size_t m_nextParamToGet = 0;
    bool m_suppressWarnings = false;
    bool m_allParseSuccess = true;
};

class CommandSystem
{
public:

    static CommandSystem* DefaultCommandSystem();

    void AddCommand( string name, CommandCallback callback );
    void RunCommand( string commandString );

    const map<string, CommandDef>& GetAllCommandDefs() const
    {
        return m_commandDefs;
    };

private:

    static CommandSystem* s_defaultCommandSystem;

    map<string, CommandDef>  m_commandDefs;
};