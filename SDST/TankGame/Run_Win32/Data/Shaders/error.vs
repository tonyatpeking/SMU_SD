// define the shader version (this is required)
#version 420 core

// Attributes - input to this shader stage (constant as far as the code is concerned)
in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;
out vec4 vert_color;
out vec2 vert_uv;

// Entry point - required.  What does this stage do?
void main( void )
{
   // for now, we're going to set the
   // clip position of this vertex to the passed
   // in position.
   // gl_Position is a "system variable", or have special
   // meaning within the shader.
   gl_Position = vec4( POSITION, 1 );sdf
   vert_color = COLOR;
   vert_uv = UV;
}