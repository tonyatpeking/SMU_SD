#pragma once
#include <string>
#include <functional>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Math/Trajectory.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/FileIO/IOUtils.hpp"
#include "Engine/Math/Solver.hpp"
#include "Engine/DataUtils/EndianUtils.hpp"
#include "Engine/DataUtils/BytePacker.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/Net.hpp"
#include "Engine/Net/TCPSocket.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Core/SystemUtils.hpp"
#include "Engine/Net/UDPSocket.hpp"

#include "Game/GameCommon.hpp"


namespace Tests
{



void PrintfTest( bool success, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    Rgba color = success ? Rgba::GREEN_CYAN : Rgba::ORANGE;
    Printfv( color, format, args );
    va_end( args );
}

void MathTests()
{
    uint16 a = 2;
    uint16 b = 3;
    uint16 c = 65530;
    uint16 d = 65531;

    bool test1 = CyclicLesser( a, b );
    bool test2 = CyclicLesser( c, d );
    bool test3 = CyclicLesser( c, a );
    bool test4 = !CyclicLesser( a, c );

    bool test5 = CyclicGreater( b, c );
    bool test6 = !CyclicGreater( c, b );



    PrintfTest( test1 && test2 && test3 && test4 && test5 && test6,
                "Cyclic Test" );
}

void SolverTests()
{
    float root1;
    float root2;
    Solver::Quadratic( 0.5f, -2.8f, 2.3f, root1, root2 );


    std::function<float( float )> eval = []( float x ) -> float {return x - 0.123456789f; };
    float bRoot = Solver::BinarySearch( 0.f, 1.f, eval, 10000, 0.00000000001f );
    UNUSED( bRoot );
};

void TrajectoryTests()
{
    Print( "Trajectory::Evaluate" );
    for( float time = 0; time < 2.f; time += 0.1f )
    {
        Vec2 pos = Trajectory::Evaluate( 10, 90, time );
        Print( ToString( pos ) );
    }
}

void StringTests()
{
    // test if IsAllWhiteSpace works
    string allWhite = " \t\t\n\r";
    string someWhite = " \t s\n df \n";
    string noWhite = "a";
    bool all = StringUtils::IsAllWhitespace( allWhite );
    bool some = StringUtils::IsAllWhitespace( someWhite );
    bool no = StringUtils::IsAllWhitespace( noWhite );
    UNUSED( all );
    UNUSED( some );
    UNUSED( no );

    StringUtils::RemoveOuterWhitespace( allWhite );
    StringUtils::RemoveOuterWhitespace( someWhite );
    StringUtils::RemoveOuterWhitespace( noWhite );

    string mockShader =
        R"(
#version 420 core

// layout binding = 0 is us binding it to texture slot 0.
layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec2 passUV;
in vec4 passColor;
/*
out vec4 outColor;
*/

/*/ //*8** / */

void main( void )
{
    //outColor = passColor;
    //outColor = vec4( vert_uv, 0.5, 1 );
    vec4 diffuse = texture( gTexDiffuse, passUV );
    outColor = diffuse * passColor;
}

)";

    StringUtils::ReplaceComments( mockShader );
    Print( mockShader );

    int someInt = 0x123456ff;
    string someIntToHex = StringUtils::ToHex( someInt );
    Print( someIntToHex );

}

void IOTests()
{

    string someDir = IOUtils::GetCurrentDir() + "/Tests/somefolder";
    string someDeepDir = IOUtils::GetCurrentDir() + "/Tests/A/B/C/D";
    string someDeepFile = IOUtils::GetCurrentDir() + "/Tests/B/C/D.txt";
    string someFile = IOUtils::GetCurrentDir() + "/Tests/somefile.txt";


    Print( "dir exists: " + ::ToString( IOUtils::DirExists( someDir ) ) );

    Print( "dir create: " + ::ToString( IOUtils::MakeDirR( someDir ) ) );

    Print( "dir exists: " + ::ToString( IOUtils::DirExists( someDir ) ) );

    Print( "dir recursive create: " + ::ToString( IOUtils::MakeDirR( someDeepDir ) ) );

    Print( "file recursive write: " + ::ToString( IOUtils::WriteToFile( someDeepFile, "hi!\nhi!" ) ) );

    Print( "file exists: " + ::ToString( IOUtils::FileExists( someFile ) ) );

}





void NetworkCourseTests()
{
    // Endian
    string end = EndianUtils::GetPlatformEndianness().ToString();
    PrintfTest( true, "Platform Endianness is: %s", end.c_str() );

    int testInt = 0x332fecae;
    int result = testInt;
    int expect = 0xaeec2f33;



    EndianUtils::ConvertEndianness( &result, Endianness::LITTLE, Endianness::BIG );
    EndianUtils::FromEndianness( &result, Endianness::BIG );
    EndianUtils::ToEndianness( &result, Endianness::BIG );
    PrintfTest( result == expect,
                "Converting %i from little to big Endian, result: %i, expected: %i",
                testInt, result, expect );

    float testFloat = 3.4f;
    float resultFloat = testFloat;
    EndianUtils::ToEndianness( &resultFloat, Endianness::BIG );
    EndianUtils::FromEndianness( &resultFloat, Endianness::BIG );
    PrintfTest( testFloat == resultFloat,
                "Converting %f from little to big Endian, then back result: %f, expected: %f",
                testFloat, resultFloat, testFloat );

    // BytePacker
    constexpr size_t maxBufferLen = 200000;
    constexpr size_t maxStrLen = maxBufferLen - 20;

    char* someStr =  (char*) malloc( maxStrLen );
    char* outStr =  (char*) malloc( maxStrLen );

    for( int i = 0; i < maxStrLen - 1; ++i )
    {
        someStr[i] = Random::Default()->Char();
    }
    someStr[maxStrLen - 1] = '\0';;


    //size_t len = strlen( someStr );

    BytePacker canGrow{};
    BytePacker fixedSize( maxBufferLen );
    void* buffer = malloc( maxBufferLen );
    BytePacker doesNotOwn( maxBufferLen, buffer );

    PrintfTest( canGrow.CanGrow() && !fixedSize.CanGrow() && !doesNotOwn.CanGrow(),
                "BytePacker memory can grow test" );

    PrintfTest( canGrow.OwnsMemory() && fixedSize.OwnsMemory() && !doesNotOwn.OwnsMemory(),
                "BytePacker memory ownership test" );


    // canGrow BytePacker read write test
    canGrow.SetEndianness( Endianness::BIG );

    int in_int = 123123;
    bool in_bool = false;
    float in_f = 2.35f;

    int out_int;
    bool out_bool;
    float out_f;

    canGrow.Write( in_int );
    canGrow.Write( in_bool );
    canGrow.Write( in_f );
    canGrow.WriteString( someStr );

    canGrow.Read( &out_int );
    canGrow.Read( &out_bool );
    canGrow.Read( &out_f );
    canGrow.ReadString( outStr, maxStrLen );

    PrintfTest( out_int == in_int,
                "BytePacker int read write test" );
    PrintfTest( out_bool == in_bool,
                "BytePacker bool read write test" );
    PrintfTest( out_f == in_f,
                "BytePacker float read write test" );

    PrintfTest( strcmp( someStr, outStr ) == 0,
                "canGrow BytePacker str read write test" );


    // fixedSize BytePacker read write test
    fixedSize.WriteString( someStr );
    fixedSize.ReadString( outStr, maxStrLen );
    PrintfTest( strcmp( someStr, outStr ) == 0,
                "fixedSize BytePacker read write test" );

    // doesNotOwn doesNotOwn read write test
    doesNotOwn.WriteString( someStr );
    doesNotOwn.ReadString( outStr, maxStrLen );
    PrintfTest( strcmp( someStr, outStr ) == 0,
                "doesNotOwn doesNotOwn read write test" );

    free( buffer );
    free( someStr );
    free( outStr );

    // Process Spawning

    PrintfTest( true,
                "My process path is %s", SystemUtils::GetExePath().c_str() );
    // lol infinite loop
//    SystemUtils::CloneProcess();
    //SystemUtils::SpawnProcess( "python" );
}

void LoggerTests()
{
    LOG_DEBUG( "LOG_DEBUG" );
    LOG_INFO( "LOG_INFO" );
    LOG_WARNING( "LOG_WARNING" );
    LOG_ERROR( "LOG_ERROR" );
    LOG_FATAL( "LOG_FATAL" );

    LOG_INFO_TAG( "TestTag", "LOG_INFO_TAG" );

}

void RunTests()
{

    Printf( Rgba::CYAN, "\n---Running Tests---\n" );
    //SolverTests();
    //TrajectoryTests();
    //StringTests();

    //IOTests();
    Printf( Rgba::CYAN, "\nNetwork Course Tests:" );
    NetworkCourseTests();

    Printf( Rgba::CYAN, "\nLogger Tests:" );
    LoggerTests();

    Printf( Rgba::CYAN, "\nMath Tests:" );
    MathTests();

    Printf( Rgba::CYAN, "\n---End of Tests---\n" );

};



}