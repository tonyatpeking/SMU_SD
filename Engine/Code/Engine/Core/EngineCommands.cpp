#include "Engine/Core/EngineCommands.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/CommandSystem.hpp"
#include "Engine/Core/Thread.hpp"
#include "Engine/IO/IOUtils.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/PythonInterpreter.hpp"
#include "Engine/Core/Console.hpp"
#include <fstream>

void ThreadTestWork()
{
    // Open a file and output about 50MB of random numbers to it;
    std::fstream file( "garbage.dat" );
    if( !file.is_open() )
    {
        return;
    }

    for( uint i = 0; i < 12000000; ++i )
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
