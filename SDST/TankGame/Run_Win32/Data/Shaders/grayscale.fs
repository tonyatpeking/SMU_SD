#version 420 core
#include "Data/Shaders/Includes/Common.glsl"

// Input Textures (color and depth)
layout(binding = 0) uniform sampler2D gTexColor;

layout(binding=4, std140) uniform uboCustom
{
    float PERCENT;
};

// Inputs (just UV and Color (fadeout can be done just with color)
in vec2 passUV;
in vec4 passColor;

// Output - a single color;
out vec4 outColor;

// this is transposed (vectors are the columns)
mat4 GRAYSCALE_TRANSFORM = {
   vec4( 0.2126, 0.7152, 0.0722, 0 ),
   vec4( 0.2126, 0.7152, 0.0722, 0 ),
   vec4( 0.2126, 0.7152, 0.0722, 0 ),
   vec4( 0,      0,      0,      1 )
};

// Entry Point
void main( void )
{
   vec4 color = texture( gTexColor, passUV );
   vec4 gray = color * GRAYSCALE_TRANSFORM;
   outColor = mix(color,gray,PERCENT);
}
