#version 420 core

#include "Data/Shaders/Includes/Common.glsl"

// Attributes
in vec3 POSITION;
in vec4 COLOR;

out vec3 passWorldPosition;
out vec4 passColor;


void main( void )
{
    // 1, since I don't want to translate
    vec4 localPos = vec4( POSITION, 0.0f );

    vec4 worldPos = MODEL * localPos; // assume local is world for now;
    passWorldPosition = worldPos.xyz;

    vec4 cameraPos = VIEW * localPos;

    // projection relies on a 1 being present, so add it back
    vec4 clipPos = PROJECTION * vec4(cameraPos.xyz, 1);

    // we only render where depth is 1.0 (equal, ie, where have we not drawn)
    // so z needs to be one for all these
    clipPos.z = clipPos.w; // z must equal w.  We set the z, not the w, so we don't affect the x and y as well

    passColor = COLOR;
	gl_Position = clipPos; // we pass out a clip coordinate
}
