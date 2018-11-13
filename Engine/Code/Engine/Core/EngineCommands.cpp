#include "Engine/Core/EngineCommands.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Core/CommandSystem.hpp"
#include "Engine/Thread/Thread.hpp"
#include "Engine/FileIO/IOUtils.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Python/PythonInterpreter.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Net/Net.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/RemoteCommandService.hpp"
#include "Engine/Core/SystemUtils.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/EngineNetMessages.hpp"
#include <fstream>
#include "Engine/Core/RuntimeVars.hpp"
#include "Engine/Core/WindowsCommon.hpp"

using namespace EngineNetMessages;

void ThreadTestWork()
{
    // Open a file and output about 50MB of random numbers to it;
    std::ofstream file;
    file.open( "big.txt" );
    if( !file.is_open() )
    {
        return;
    }

    for( uint i = 0; i < 1200000; ++i )
    {
        int randInt = Random::Default()->IntInRange( 0, 99999 );
        file << randInt;
    }

    file.close();
    DebuggerPrintf( "Finished ThreadTestWork" );
}


void EngineCommands::RegisterAllCommands()
{
    CommandSystem* commandSys = CommandSystem::DefaultCommandSystem();

    //--------------------------------------------------------------------------------------
    // Process

    commandSys->AddCommand( "clone", []( string& str )
    {
        CommandParameterParser parser( str );

        size_t numParams = parser.NumOfParams();
        if( numParams == 0 )
        {
            SystemUtils::CloneProcess();
            return;
        }
        if( numParams == 1 )
        {
            int count;
            parser.GetNext( count );
            if( count > 20 )
                count = 20;
            for( int idx = 0; idx < count; ++idx )
            {
                SystemUtils::CloneProcess();
            }
            return;
        }
    } );


    commandSys->AddCommand( "spawn", []( string& str )
    {
        CommandParameterParser parser( str );

        size_t numParams = parser.NumOfParams();
        if( numParams == 1 )
        {
            string path;
            parser.GetNext( path );
            SystemUtils::SpawnProcess( path );
            return;
        }
        if( numParams == 2 )
        {
            int count;
            parser.GetNext( count );
            if( count > 20 )
                count = 20;
            string path;
            parser.GetNext( path );
            for( int idx = 0; idx < count; ++idx )
            {
                SystemUtils::SpawnProcess( path );
            }
            return;
        }
    } );

    //--------------------------------------------------------------------------------------
    // Thread

    commandSys->AddCommand( "testThreaded", []( string& str )
    {
        UNUSED( str );
        Thread::CreateAndDetach( ThreadTestWork );
    } );

    commandSys->AddCommand( "testNonThreaded", []( string& str )
    {
        UNUSED( str );
        ThreadTestWork();
    } );

    commandSys->AddCommand( "testThreadJoin", []( string& str )
    {
        UNUSED( str );
        Thread::Handle handle = Thread::Create( ThreadTestWork );
        Thread::Join( handle );
    } );

    //--------------------------------------------------------------------------------------
    // Profiler

    commandSys->AddCommand( "p", []( string& str )
    {
        UNUSED( str );
        Profiler::ToggleVisible();
    } );


    commandSys->AddCommand( "pp", []( string& str )
    {
        UNUSED( str );
        Print( "Paused profiler" );
        Profiler::Pause();
    } );


    commandSys->AddCommand( "pr", []( string& str )
    {
        UNUSED( str );
        Print( "Resumed profiler" );
        Profiler::Resume();
    } );

    //--------------------------------------------------------------------------------------
    // Python

    commandSys->AddCommand( "python", []( string& str )
    {
        UNUSED( str );
        Print( "Starting Python Shell" );
        Console::GetDefault()->UsePython( true );
    } );


    commandSys->AddCommand( "py", []( string& str )
    {
        UNUSED( str );
        Print( "Starting Python Shell" );
        Console::GetDefault()->UsePython( true );
    } );

    //--------------------------------------------------------------------------------------
    // Utils

    commandSys->AddCommand( "help", []( string& str )
    {
        UNUSED( str );
        const map<string, CommandDef>& allCommandDefs =
            CommandSystem::DefaultCommandSystem()->GetAllCommandDefs();
        for( auto& CommandDef : allCommandDefs )
        {
            Print( CommandDef.second.m_name, Rgba::GREEN_CYAN );
        }
    } );


    //--------------------------------------------------------------------------------------
    // Net commands
    commandSys->AddCommand( "TestConnect", []( string& str )
    {
        UNUSED( str );
        CommandParameterParser parser( str );
        size_t numParams = parser.NumOfParams();
        if( numParams < 2 )
        {
            LOG_WARNING( "TestConnect needs 2 params, IP:PORT and msg" );
            return;
        }
        string addrStr;
        parser.GetNext( addrStr );
        NetAddress netAddr( addrStr );
        string msg;
        parser.GetNext( msg );
        Net::ConnectionTest( netAddr, msg );
    } );


    commandSys->AddCommand( "TestHost", []( string& str )
    {
        UNUSED( str );
        CommandParameterParser parser( str );

        size_t numParams = parser.NumOfParams();
        if( numParams < 1 )
        {
            LOG_WARNING( "TestHost needs 1 params, PORT" );
            return;
        }

        int port;
        parser.GetNext( port );

        Net::HostTest( port );

    } );


    commandSys->AddCommand( "TestHostClose", []( string& str )
    {
        UNUSED( str );

        Net::HostTestClose();

    } );


    commandSys->AddCommand( "NetLocalIP", []( string& str )
    {
        UNUSED( str );

        vector<NetAddress> addresses = NetAddress::GetAllLocal( "12345" );

        for( auto& addr : addresses )
        {
            Print( "Local IP: " + addr.ToStringIP() );
        }

    } );


    commandSys->AddCommand( "rc", []( string& str )
    {
        CommandParameterParser parser( str );
        size_t numParams = parser.NumOfParams();
        if( numParams < 1 )
        {
            LOG_WARNING( "rc needs at least 1 param, (optional idx) and msg" );
            return;
        }
        string onlyParams = "";
        int indexToSend = 0;
        for( int idx = 0; idx < numParams; ++idx )
        {
            string param;
            parser.GetNext( param );
            if( idx == 0 )
            {
                if( StringUtils::IsPositiveInt( param ) )
                {
                    if( numParams < 2 )
                    {
                        LOG_WARNING( "rc needs at least 2 param if supplying idx, (optional idx) and msg" );
                        return;
                    }
                    SetFromString( param, indexToSend );
                }
                else
                {
                    onlyParams = param;
                }
            }
            else
            {
                if( onlyParams == "" )
                    onlyParams = param;
                else
                    onlyParams = onlyParams + " " + param;
            }
        }
        RemoteCommandService::GetDefault()->SendMsg( indexToSend, false, onlyParams.c_str() );
    } );


    commandSys->AddCommand( "rca", []( string& str )
    {
        CommandParameterParser parser( str );
        size_t numParams = parser.NumOfParams();
        if( numParams < 1 )
        {
            LOG_WARNING( "rca needs at least 1 param" );
            return;
        }
        string onlyParams = parser.GetOnlyParameters();
        RemoteCommandService::GetDefault()->RemoteCommandAll( onlyParams.c_str() );
    } );


    commandSys->AddCommand( "rcb", []( string& str )
    {
        CommandParameterParser parser( str );
        size_t numParams = parser.NumOfParams();
        if( numParams < 1 )
        {
            LOG_WARNING( "rcb needs at least 1 param" );
            return;
        }

        string onlyParams = parser.GetOnlyParameters();
        RemoteCommandService::GetDefault()->RemoteCommandAllButMe( onlyParams.c_str() );
    } );


    commandSys->AddCommand( "rc_host", []( string& str )
    {
        CommandParameterParser parser( str );
        size_t numParams = parser.NumOfParams();
        if( numParams > 1 )
        {
            LOG_WARNING( "rc_host needs at most 1 param, [service]" );
            return;
        }
        string onlyParams = parser.GetOnlyParameters();
        RemoteCommandService::GetDefault()->ShouldHost( onlyParams.c_str() );
    } );


    commandSys->AddCommand( "rc_join", []( string& str )
    {
        CommandParameterParser parser( str );
        size_t numParams = parser.NumOfParams();
        if( numParams != 1 )
        {
            LOG_WARNING( "rc_join needs 1 param, [ipAddress:port]" );
            return;
        }
        string onlyParams = parser.GetOnlyParameters();
        RemoteCommandService::GetDefault()->ShouldJoin( onlyParams.c_str() );
    } );


    commandSys->AddCommand( "rc_echo", []( string& str )
    {
        CommandParameterParser parser( str );
        size_t numParams = parser.NumOfParams();
        if( numParams != 1 )
        {
            LOG_WARNING( "rc_echo needs 1 param, [true/false]" );
            return;
        }
        bool on;
        parser.GetNext( on );
        Printf( "rc echo is %s", ToString( on ).c_str() );
        RemoteCommandService::GetDefault()->SetEchoOn( on );
    } );


    commandSys->AddCommand( "add_connection", []( string& str )
    {
        CommandParameterParser parser( str );
        int connectionIdx;
        string address;
        if( !parser.GetNext( connectionIdx ) || !parser.GetNext( address ) )
            return;

        NetAddress addr = NetAddress( address );
        NetSession::GetDefault()->AddConnection( (uint8) connectionIdx, addr );
    } );

    commandSys->AddCommand( "add_connection_self", []( string& str )
    {
        CommandParameterParser parser( str );
        int connectionIdx;
        if( !parser.GetNext( connectionIdx ) )
            return;

        NetSession::GetDefault()->AddConnectionSelf( (uint8) connectionIdx );
    } );

    commandSys->AddCommand( "send_ping", []( string& str )
    {
        CommandParameterParser parser( str );
        int connectionIdx;
        string txt = "";
        if( !parser.GetNext( connectionIdx ) )
            return;
        parser.GetNext( txt );

        NetMessage* msg = Compose_Ping( txt );
        NetSession::GetDefault()->SendToConnection( (uint8) connectionIdx, msg );
    } );

    commandSys->AddCommand( "send_add", []( string& str )
    {
        CommandParameterParser parser( str );
        int connectionIdx;
        float a, b;
        string txt = "";
        if( !parser.GetNext( connectionIdx )
            || !parser.GetNext( a )
            || !parser.GetNext( b ) )
            return;


        NetMessage* msg = Compose_Add( a, b );
        NetSession::GetDefault()->SendToConnection( (uint8) connectionIdx, msg );
    } );

    commandSys->AddCommand( "net_sim_lag", []( string& str )
    {
        CommandParameterParser parser( str );
        int maxLag = 0;
        int minLag;
        if( !parser.GetNext( minLag ) )
        {
            LOG_INVALID_PARAMETERS( "net_sim_loss" );
            return;
        }

        parser.GetNext( maxLag );

        NetSession::GetDefault()->SetSimLatency( minLag, maxLag );
    } );

    commandSys->AddCommand( "net_sim_loss", []( string& str )
    {
        CommandParameterParser parser( str );
        float loss;
        if( !parser.GetNext( loss ) )
        {
            LOG_INVALID_PARAMETERS( "net_sim_loss" );
            return;
        }

        NetSession::GetDefault()->SetSimLoss( loss );

    } );

    commandSys->AddCommand( "net_set_session_send_rate", []( string& str )
    {
        CommandParameterParser parser( str );
        float Hz;
        if( !parser.GetNext( Hz ) )
        {
            LOG_INVALID_PARAMETERS( "net_set_session_send_rate" );
            return;
        }

        NetSession::GetDefault()->SetSessionSendRate( Hz );

    } );

    commandSys->AddCommand( "net_set_connection_send_rate", []( string& str )
    {
        CommandParameterParser parser( str );
        int connectionIdx;
        float Hz;
        if( !parser.GetNext( connectionIdx ) || !parser.GetNext( Hz ) )
        {
            LOG_INVALID_PARAMETERS( "net_set_connection_send_rate" );
            return;
        }

        NetSession::GetDefault()->SetConnectionSendRate( (uint8) connectionIdx, Hz );

    } );

    commandSys->AddCommand( "net_set_heart_rate", []( string& str )
    {
        CommandParameterParser parser( str );
        float Hz;
        if( !parser.GetNext( Hz ) )
        {
            LOG_INVALID_PARAMETERS( "net_set_heart_rate" );
            return;
        }
        NetSession::GetDefault()->SetHeartBeat( Hz );
    } );

    commandSys->AddCommand( "net_easy_add", []( string& str )
    {
        CommandParameterParser parser( str );

        string localIP1 = NetAddress::GetLocal( ToString( GAME_PORT ) ).ToStringAll();
        string localIP2 = NetAddress::GetLocal( ToString( GAME_PORT + 1 ) ).ToStringAll();
        CommandSystem::DefaultCommandSystem()->RunCommand( "rca add_connection 0 " + localIP1 );
        CommandSystem::DefaultCommandSystem()->RunCommand( "rca add_connection 1 " + localIP2 );
    } );

    commandSys->AddCommand( "net_easy_add2", []( string& str )
    {
        CommandParameterParser parser( str );

        string localIP1 = NetAddress::GetLocal( ToString( GAME_PORT ) ).ToStringAll();
        string localIP2 = NetAddress::GetLocal( ToString( GAME_PORT + 1 ) ).ToStringAll();
        CommandSystem::DefaultCommandSystem()->RunCommand( "add_connection 0 " + localIP1 );
        CommandSystem::DefaultCommandSystem()->RunCommand( "rca add_connection 1 " + localIP2 );
    } );

    commandSys->AddCommand( "net_easy_add3", []( string& str )
    {
        CommandParameterParser parser( str );

        string localIP1 = NetAddress::GetLocal( ToString( GAME_PORT ) ).ToStringAll();
        string localIP2 = NetAddress::GetLocal( ToString( GAME_PORT + 1 ) ).ToStringAll();
        CommandSystem::DefaultCommandSystem()->RunCommand( "rca add_connection 0 " + localIP1 );
        CommandSystem::DefaultCommandSystem()->RunCommand( "add_connection 1 " + localIP2 );
    } );

    commandSys->AddCommand( "logVars", []( string& str )
    {
        CommandParameterParser parser( str );

        RuntimeVars::LogAll();
    } );

    commandSys->AddCommand( "setBool", []( string& str )
    {
        CommandParameterParser parser( str );

        string var;
        bool value;
        if( parser.GetNext( var ) && parser.GetNext( value ) )
            RuntimeVars::SetVar( var, value );
        else
            LOG_INVALID_PARAMETERS( "set command take 2 args" );
    } );

    commandSys->AddCommand( "getBool", []( string& str )
    {
        CommandParameterParser parser( str );

        string var;
        bool value;
        if( parser.GetNext( var ) )
        {
            RuntimeVars::GetVar( var, value );
            LOG_INFO_TAG( "VARS", "%s, %s", var.c_str(), ToString( value ).c_str() );
        }
        else
            LOG_INVALID_PARAMETERS( "get command take 1 args" );
    } );

    commandSys->AddCommand( "setInt", []( string& str )
    {
        CommandParameterParser parser( str );

        string var;
        int value;
        if( parser.GetNext( var ) && parser.GetNext( value ) )
            RuntimeVars::SetVar( var, value );
        else
            LOG_INVALID_PARAMETERS( "set command take 2 args" );
    } );

    commandSys->AddCommand( "getInt", []( string& str )
    {
        CommandParameterParser parser( str );

        string var;
        int value;
        if( parser.GetNext( var ) )
        {
            RuntimeVars::GetVar( var, value );
            LOG_INFO_TAG( "VARS", "%s, %i", var.c_str(), ToString( value ).c_str() );
        }
        else
            LOG_INVALID_PARAMETERS( "get command take 1 args" );
    } );

    commandSys->AddCommand( "setFloat", []( string& str )
    {
        CommandParameterParser parser( str );

        string var;
        float value;

        if( parser.GetNext( var ) && parser.GetNext( value ) )
            RuntimeVars::SetVar( var, value );
        else
            LOG_INVALID_PARAMETERS( "set command take 2 args" );
    } );

    commandSys->AddCommand( "getFloat", []( string& str )
    {
        CommandParameterParser parser( str );

        string var;
        float value;
        if( parser.GetNext( var ) )
        {
            RuntimeVars::GetVar( var, value );
            LOG_INFO_TAG( "VARS", "%s, %f", var.c_str(), ToString( value ).c_str() );
        }
        else
            LOG_INVALID_PARAMETERS( "get command take 1 args" );
    } );

    commandSys->AddCommand( "setString", []( string& str )
    {
        CommandParameterParser parser( str );

        string var;
        string value;
        if( parser.GetNext( var ) && parser.GetNext( value ) )
            RuntimeVars::SetVar( var, value );
        else
            LOG_INVALID_PARAMETERS( "set command take 2 args" );
    } );

    commandSys->AddCommand( "getString", []( string& str )
    {
        CommandParameterParser parser( str );

        string var;
        string value;
        if( parser.GetNext( var ) )
        {
            RuntimeVars::GetVar( var, value );
            LOG_INFO_TAG( "VARS", "%s, %s", var.c_str(), ToString( value ).c_str() );
        }
        else
            LOG_INVALID_PARAMETERS( "get command take 1 args" );
    } );

}


