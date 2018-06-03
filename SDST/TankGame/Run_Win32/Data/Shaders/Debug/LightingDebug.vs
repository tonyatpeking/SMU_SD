#version 420 core

#include "Data/Shaders/Includes/Common.glsl"

in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;
in vec3 NORMAL;
in vec4 TANGENT;

out vec4 passColor;
out vec2 passUV;
out vec3 passWorldNormal;
out vec3 passWorldTangent;
out vec3 passWorldBitangent;
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
    passWorldTangent = ( MODEL * vec4( TANGENT.xyz, 0.0f )).xyz;
	passWorldBitangent = cross( passWorldTangent, passWorldNormal ) * TANGENT.w;
    passWorldPos = worldPos.xyz;
}