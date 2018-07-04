layout(binding=1, std140) uniform uboTimeClock
{
	float GAME_DELTA_TIME;
	float GAME_TIME;
	float SYSTEM_DELTA_TIME;
	float SYSTEM_TIME;
};

layout(binding=2, std140) uniform uboCamera
{
	mat4 VIEW;
	mat4 PROJECTION;
	mat4 VP;
	vec3 CAMERA_POSITION;
	float PAD_CAM_0;
};

layout(binding=3, std140) uniform uboInstance
{
	mat4 MODEL;
	mat4 MVP;
	vec4 TINT;
	float SPECULAR_AMOUNT;
	float SPECULAR_POWER;
	float PAD_INST_0;
	float PAD_INST_1;
};

layout(binding=5, std140) uniform uboGlobal
{
	// Fog
	vec4 FOG_COLOR; 
	float FOG_NEAR_PLANE;
	float FOG_NEAR_FACTOR;
	float FOG_FAR_PLANE;
	float FOG_FAR_FACTOR;

	mat4 SHADOW_MAP_VP;
    mat4 SHADOW_MAP_INV_VP;

	vec2 WINDOW_SIZE;



};

struct Light
{
	vec4 color; // alpha is intensity

    vec3 position;
    float isPointLight;  // 1 for point light, 0 for directional light

	vec3 direction; // the direction the light is pointing
    float sourceRadius; // the physical size of the light, affects attenuation

    float coneInnerDot;
    float coneOuterDot;
    float pad1;
    float pad0;
};

//MAX_LIGHTS can be set in cpp with define injection
#ifndef MAX_LIGHTS
#define MAX_LIGHTS (8)
#endif

layout(binding=6, std140) uniform uboLights
{
	vec4 AMBIENT;  // xyz color, w intensity
	Light LIGHTS[MAX_LIGHTS];
};