#version 420 core

in vec2 passUV;
in vec4 passColor;
out vec4 outColor;

void main( void )
{
    outColor = vec4( passUV, 1, 1 );

}