#version 420 core


// Suggest always manually setting bindings - again, consitancy with
// other rendering APIs and well as you can make assumptions in your
// engine without having to query
layout(binding = 0) uniform samplerCube gTexSky;

in vec3 passWorldPosition;
in vec4 passColor;

out vec4 outColor;

void main( void )
{
   vec4 texColor = texture( gTexSky, passWorldPosition );

   outColor = texColor * passColor;
}
