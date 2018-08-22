#version 420 core

#include "Data/Shaders/Includes/Common.glsl"

in vec3 POSITION;


void main( void )
{
    vec4 localPos = vec4( POSITION, 1 );
    vec4 clipPos = MVP * localPos;

    gl_Position = clipPos;
}