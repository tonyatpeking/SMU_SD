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
#include <fstream>

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

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

    commandSys->AddCommand( "rc", []( String str )
    {
        CommandParameterParser parser( str );

        size_t numParams = parser.NumOfParams();
        if( numParams < 1 )
        {
            LOG_WARNING( "rc needs at least 1 param, (optional idx) and msg" );
            return;
        }

        String onlyParams = "";


        int indexToSend = 0;

        for (int idx = 0; idx < numParams; ++idx)
        {
            String param;
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


    commandSys->AddCommand( "rca", []( String str )
    {
        CommandParameterParser parser( str );

        size_t numParams = parser.NumOfParams();
        if( numParams < 1 )
        {
            LOG_WARNING( "rca needs at least 1 param" );
            return;
        }

        String onlyParams = parser.GetOnlyParameters();

        RemoteCommandService::GetDefault()->RemoteCommandAll( onlyParams.c_str() );
    } );

    commandSys->AddCommand( "rcb", []( String str )
    {
        CommandParameterParser parser( str );

        size_t numParams = parser.NumOfParams();
        if( numParams < 1 )
        {
            LOG_WARNING( "rcb needs at least 1 param" );
            return;
        }

        String onlyParams = parser.GetOnlyParameters();

        RemoteCommandService::GetDefault()->RemoteCommandAllButMe( onlyParams.c_str() );
    } );

    commandSys->AddCommand( "rcs_host", []( String str )
    {
        CommandParameterParser parser( str );

        size_t numParams = parser.NumOfParams();
        if( numParams > 1 )
        {
            LOG_WARNING( "rcs_host needs at most 1 param, [service]" );
            return;
        }

        String onlyParams = parser.GetOnlyParameters();

        RemoteCommandService::GetDefault()->ShouldHost( onlyParams.c_str() );
    } );

    commandSys->AddCommand( "rcs_join", []( String str )
    {
        CommandParameterParser parser( str );

        size_t numParams = parser.NumOfParams();
        if( numParams != 1 )
        {
            LOG_WARNING( "rcs_join needs 1 param, [ipAddress:port]" );
            return;
        }

        String onlyParams = parser.GetOnlyParameters();

        RemoteCommandService::GetDefault()->ShouldJoin( onlyParams.c_str() );
    } );

    commandSys->AddCommand( "rcs_echo", []( String str )
    {
        CommandParameterParser parser( str );

        size_t numParams = parser.NumOfParams();
        if( numParams != 1 )
        {
            LOG_WARNING( "rcs_echo needs 1 param, [true/false]" );
            return;
        }


        bool on;

        parser.GetNext( on );

        Console::DefaultConsole()->Printf( "rcs echo is %s", ToString(on).c_str() );

        RemoteCommandService::GetDefault()->SetEchoOn( on );
    } );



    commandSys->AddCommand( "TestConnect", []( String str )
    {
        UNUSED( str );
        CommandParameterParser parser( str );

        size_t numParams = parser.NumOfParams();
        if( numParams < 2 )
        {
            LOG_WARNING( "TestConnect needs 2 params, IP:PORT and msg" );
            return;
        }

        String addrStr;
        parser.GetNext( addrStr );
        NetAddress netAddr( addrStr );

        String msg;
        parser.GetNext( msg );

        Net::ConnectionTest( netAddr, msg );

    } );

    commandSys->AddCommand( "TestHost", []( String str )
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

    commandSys->AddCommand( "TestHostClose", []( String str )
    {
        UNUSED( str );

        Net::HostTestClose();

    } );

    commandSys->AddCommand( "NetLocalIP", []( String str )
    {
        UNUSED( str );

        std::vector<NetAddress> addresses = NetAddress::GetAllLocal( "12345" );

        for( auto& addr : addresses )
        {
            Console::DefaultConsole()->Print( "Local IP: " + addr.ToStringIP() );
        }

    } );

    commandSys->AddCommand( "testThreaded", []( String str )
    {
        UNUSED( str );
        Thread::CreateAndDetach( ThreadTestWork );
    } );

    commandSys->AddCommand( "testNonThreaded", []( String str )
    {
        UNUSED( str );
        ThreadTestWork();
    } );

    commandSys->AddCommand( "testThreadJoin", []( String str )
    {
        UNUSED( str );
        Thread::Handle handle = Thread::Create( ThreadTestWork );
        Thread::Join( handle );
    } );

    commandSys->AddCommand( "p", []( String str )
    {
        UNUSED( str );
        Profiler::ToggleVisible();
    } );

    commandSys->AddCommand( "pp", []( String str )
    {
        UNUSED( str );
        Console::DefaultConsole()->Print( "Paused profiler" );
        Profiler::Pause();
    } );

    commandSys->AddCommand( "pr", []( String str )
    {
        UNUSED( str );
        Console::DefaultConsole()->Print( "Resumed profiler" );
        Profiler::Resume();
    } );

    commandSys->AddCommand( "python", []( String str )
    {
        UNUSED( str );
        Console::DefaultConsole()->Print( "Starting Python Shell" );
        Console::DefaultConsole()->UsePython( true );
    } );

    commandSys->AddCommand( "py", []( String str )
    {
        UNUSED( str );
        Console::DefaultConsole()->Print( "Starting Python Shell" );
        Console::DefaultConsole()->UsePython( true );
    } );

    commandSys->AddCommand( "help", []( String str )
    {
        const std::map<String, CommandDef>& allCommandDefs =
            CommandSystem::DefaultCommandSystem()->GetAllCommandDefs();
        for( auto& CommandDef : allCommandDefs )
        {
            Console::DefaultConsole()->Print( CommandDef.second.m_name, Rgba::GREEN_CYAN );
        }
    } );
}
