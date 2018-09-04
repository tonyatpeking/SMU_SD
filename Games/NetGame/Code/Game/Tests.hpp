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
    g_console->Print( "Trajectory::Evaluate" );
    for( float time = 0; time < 2.f; time += 0.1f )
    {
        Vec2 pos = Trajectory::Evaluate( 10, 90, time );
        g_console->Print( ToString( pos ) );
    }
}

void StringTests()
{
    // test if IsAllWhiteSpace works
    String allWhite = " \t\t\n\r";
    String someWhite = " \t s\n df \n";
    String noWhite = "a";
    bool all = StringUtils::IsAllWhitespace( allWhite );
    bool some = StringUtils::IsAllWhitespace( someWhite );
    bool no = StringUtils::IsAllWhitespace( noWhite );
    UNUSED( all );
    UNUSED( some );
    UNUSED( no );

    StringUtils::RemoveOuterWhitespace( allWhite );
    StringUtils::RemoveOuterWhitespace( someWhite );
    StringUtils::RemoveOuterWhitespace( noWhite );

    String mockShader =
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
    g_console->Print( mockShader );

}

void IOTests()
{

    String someDir = IOUtils::GetCurrentDir() + "/Tests/somefolder";
    String someDeepDir = IOUtils::GetCurrentDir() + "/Tests/A/B/C/D";
    String someDeepFile = IOUtils::GetCurrentDir() + "/Tests/B/C/D.txt";
    String someFile = IOUtils::GetCurrentDir() + "/Tests/somefile.txt";


    g_console->Print( "dir exists: " + ::ToString( IOUtils::DirExists( someDir ) ) );

    g_console->Print( "dir create: " + ::ToString( IOUtils::MakeDirR( someDir ) ) );

    g_console->Print( "dir exists: " + ::ToString( IOUtils::DirExists( someDir ) ) );

    g_console->Print( "dir recursive create: " + ::ToString( IOUtils::MakeDirR( someDeepDir ) ) );

    g_console->Print( "file recursive write: " + ::ToString( IOUtils::WriteToFile( someDeepFile, "hi!\nhi!" ) ) );

    g_console->Print( "file exists: " + ::ToString( IOUtils::FileExists( someFile ) ) );

}

void NetworkCourseTests()
{
    // Endian
    String end = EndianUtils::GetPlatformEndianness().ToString();
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
        someStr[i] = Random::Char();
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

    Printf( Rgba::CYAN, "\n---End of Tests---\n" );

};



}