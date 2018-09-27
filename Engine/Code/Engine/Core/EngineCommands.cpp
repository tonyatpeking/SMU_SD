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
#include <fstream>

#include "Engine/Core/WindowsCommon.hpp"

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
        int randInt = Random::IntInRange( 0, 99999 );
        file << randInt;
    }

    file.close();
    DebuggerPrintf( "Finished ThreadTestWork" );
}


void EngineCommands::RegisterAllCommands()
{
    CommandSystem* commandSys = CommandSystem::DefaultCommandSystem();

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

    commandSys->AddCommand( "clone_process", []( string& str )
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

    commandSys->AddCommand( "spawn_process", []( string& str )
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
}
