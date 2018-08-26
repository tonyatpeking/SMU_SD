#pragma once
#include <string>
#include <functional>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Math/Trajectory.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/IO/IOUtils.hpp"
#include "Engine/Math/Solver.hpp"

#include "Game/GameCommon.hpp"

namespace Tests
{


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

void RunTests()
{
    SolverTests();
    //TrajectoryTests();
    StringTests();

    IOTests();
};

// each update test is responsible of fetching its own clock
void UpdateTests()
{

}

void RenderTests()
{

}

}