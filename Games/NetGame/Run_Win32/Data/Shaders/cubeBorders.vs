#version 420 core

uniform mat4 PROJECTION;
uniform mat4 MVP;
in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;
out vec4 passColor;
out vec2 passUV;
out vec2 faceUV;

void main( void )
{
    vec4 localPos = vec4( POSITION, 1 );
    vec4 clipPos = MVP * localPos;
    gl_Position = clipPos;
    passColor = COLOR;
    passUV = UV;
    //3 2
    //0 1

    //2 3
    //0 1
    int modIdx = int(mod(gl_VertexID,4));
    int xComp =  int(mod(modIdx,2 ));
    int yComp = modIdx / 2;
    faceUV = vec2( xComp, yComp);
}
