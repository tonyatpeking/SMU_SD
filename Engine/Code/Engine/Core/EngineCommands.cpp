#include "Engine/Core/EngineCommands.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/CommandSystem.hpp"
#include "Engine/Core/Thread.hpp"
#include "Engine/IO/IOUtils.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/PythonInterpreter.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Net/Net.hpp"
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
        int randInt = Random::IntInRange(0,99999);
        file << randInt;
    }

    file.close();
    DebuggerPrintf( "Finished ThreadTestWork" );
}


void EngineCommands::RegisterAllCommands()
{
    CommandSystem* commandSys = CommandSystem::DefaultCommandSystem();

    commandSys->AddCommand( "netAddr", []( String str )
    {
        UNUSED( str );

        sockaddr_storage out_sockaddr;
        int addrlen;

//         char *hostname = "10.8.139.114";
//         char* service = "12345";

        char *hostname = "neesarg.me";
        char* service = "80";

        Net::GetAddressForHost( (sockaddr*)(&out_sockaddr), &addrlen, hostname, service );

        char out[256];
        int i = 1;
//         inet_ntop( ipv4->sin_family, &( ipv4->sin_addr ), out, 256 );
//         LogTaggedPrintf( "net", "My Address: %s", out );


        //void ConnectExample( net_address_t const &addr, char const *msg )


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
