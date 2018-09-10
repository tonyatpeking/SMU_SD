#pragma once
#include <string>
#include <vector>
#include <map>
#include "Engine/String/StringUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"



typedef void( *CommandCallback )( String );

// make a static variable of this to self register to the default command system
class CommandSelfRegister
{
    CommandSelfRegister( String name, CommandCallback callback );
    ~CommandSelfRegister() {};
};

struct CommandDef
{
public:

    CommandDef() {}

    CommandDef( String name, CommandCallback callback )
        : m_name( name )
        , m_callback( callback )
    {};

    String m_name;

    CommandCallback m_callback = nullptr;
};

class CommandParameterParser
{
public:

    CommandParameterParser( const String& commandString, bool suppressWarnings = false );

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
                LOG_INVALID_PARAMETERS( m_commandName.c_str() );
            m_allParseSuccess = false;
        }

        return false;
    };

    bool AllParseSuccess() { return m_allParseSuccess; };
    size_t NumOfParams() { return m_parameterTokens.size(); };
    String GetName() { return m_commandName; };
    String GetOnlyParameters() { return m_justParams; };
private:
    Strings m_parameterTokens;
    String m_commandName;
    String m_justParams;
    size_t m_nextParamToGet = 0;
    bool m_suppressWarnings;
    bool m_allParseSuccess = true;
};

class CommandSystem
{
public:

    static CommandSystem* DefaultCommandSystem();

    void AddCommand( String name, CommandCallback callback );
    void RunCommand( String commandString );

    const std::map<String, CommandDef>& GetAllCommandDefs() const
    {
        return m_commandDefs;
    };

private:

    static CommandSystem* s_defaultCommandSystem;

    std::map<String, CommandDef>  m_commandDefs;
};