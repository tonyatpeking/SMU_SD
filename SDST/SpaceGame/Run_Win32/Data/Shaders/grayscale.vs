#version 420 core

// Attributes
in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;

out vec2 passUV;
out vec4 passColor;

void main( void )
{
    vec4 clip_pos = vec4( POSITION, 1.0f );

    passUV = UV;
    passColor = COLOR;
    gl_Position = clip_pos; // we pass out a clip coordinate
}
