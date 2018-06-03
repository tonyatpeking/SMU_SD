#pragma once
#include <string>

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Math/Trajectory.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Game/GameCommon.hpp"

namespace Tests
{


void QuadraticTests()
{
    float root1;
    float root2;
    Solver::Quadratic( 0.5f, -2.8f, 2.3f, root1, root2 );
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

void RunTests()
{
    QuadraticTests();
    //TrajectoryTests();
    StringTests();

};

// each update test is responsible of fetching its own clock
void UpdateTests()
{

}

void RenderTests()
{

}

}