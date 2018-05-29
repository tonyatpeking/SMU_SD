#version 420 core

uniform mat4 PROJECTION;
in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;
out vec4 passColor;
out vec2 passUV;

void main( void )
{
    vec4 localPos = vec4( POSITION, 1 );
    vec4 clipPos = PROJECTION * localPos;
    gl_Position = clipPos;
    passColor = COLOR;
    passUV = UV;
}