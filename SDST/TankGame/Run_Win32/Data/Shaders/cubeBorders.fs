#version 420 core

// layout binding = 0 is us binding it to texture slot 0.
layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec2 passUV;
in vec2 faceUV;
in vec4 passColor;
out vec4 outColor;

void main( void )
{
    //outColor = passColor;
    //outColor = vec4( faceUV, 0, 1 );
    vec4 diffuse = texture( gTexDiffuse, passUV );
    // burnX is small at the ends of [0,1]
    float burnX = (faceUV.x - 0.5) * 2;
    burnX = 1 - (burnX * burnX * burnX * burnX);
    float burnY = (faceUV.y - 0.5) * 2;
    burnY = 1 - (burnY * burnY * burnY * burnY);
    float burn = burnX * burnY;
    vec4 diffuseBurned = vec4( diffuse.rgb * burnX * burnY, diffuse.a);
    outColor = diffuse * passColor;
    outColor = diffuseBurned;
}