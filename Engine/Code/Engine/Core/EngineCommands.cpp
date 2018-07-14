#include "Engine/Core/EngineCommands.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/CommandSystem.hpp"
#include "Engine/Core/Thread.hpp"
#include "Engine/IO/IOUtils.hpp"
#include "Engine/Math/Random.hpp"
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

}
