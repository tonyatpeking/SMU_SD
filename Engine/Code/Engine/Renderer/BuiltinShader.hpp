#pragma once

// this is where the generated shader will be put
constexpr char* DEBUG_GENERATE_PATH = "Data/Shaders/Generated/Debug";
constexpr char* DEBUG_SHADER = "DEBUG_SHADER";


constexpr char* BUILTIN_GENERATE_PATH = "Data/Shaders/Generated/Builtin";
constexpr char* BUILTIN_SHADER_FILE = __FILE__;
constexpr char* BUILTIN_SHADER = "BUILTIN_SHADER";

constexpr int BUILTIN_VERTEX_SHADER_LINE = __LINE__ + 3;
constexpr char* BUILTIN_VERTEX_SHADER =
R"(
#version 420 core

#include "Data/Shaders/Includes/Common.glsl"

in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;
in vec3 NORMAL;

out vec4 passColor;
out vec2 passUV;
out vec3 passWorldNormal;
out vec3 passWorldPos;

void main( void )
{
    vec4 localPos = vec4( POSITION, 1 );
    vec4 worldPos = MODEL * localPos;
    vec4 cameraPos = VIEW * worldPos;
    vec4 clipPos = PROJECTION * cameraPos;

    gl_Position = clipPos;

    passColor = COLOR * TINT;
    passUV = UV;
    passWorldNormal = ( MODEL * vec4( NORMAL, 0.0f )).xyz;
    passWorldPos = worldPos.xyz;
}

)";


constexpr int BUILTIN_FRAGMENT_SHADER_LINE = __LINE__ + 3;
constexpr char* BUILTIN_FRAGMENT_SHADER =
R"(
#version 420 core

#include "Data/Shaders/Includes/Common.glsl"

// layout binding = 0 is us binding it to texture slot 0.
layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec4 passColor;
in vec2 passUV;
in vec3 passWorldNormal;
in vec3 passWorldPos;

out vec4 outColor;

void main( void )
{
    vec4 diffuse = texture( gTexDiffuse, passUV );
    //vec3 normalColor = ( passWorldNormal + 1 ) / 2;
    outColor = diffuse * passColor;
    //outColor = vec4( normalColor, 1 );
}

)";

// this is where the generated shader will be put
constexpr char* INVALID_GENERATE_PATH = "Data/Shaders/Generated/Invalid";
constexpr char* INVALID_SHADER = "INVALID_SHADER";
constexpr int INVALID_VERTEX_SHADER_LINE = __LINE__ + 3;
constexpr char* INVALID_VERTEX_SHADER =
R"(
#version 420 core

in vec3 POSITION;

void main( void )
{
    gl_Position = vec4( POSITION, 1 );
}

)";

constexpr int INVALID_FRAGMENT_SHADER_LINE = __LINE__ + 3;
constexpr char* INVALID_FRAGMENT_SHADER =
R"(
#version 420 core

out vec4 outColor;

void main( void )
{
    outColor = vec4(1,0,1,1);
}

)";
