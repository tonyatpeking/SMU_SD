
//--------------------------------------------------------------------------------------
// FOG
//--------------------------------------------------------------------------------------

#include "Data/Shaders/Includes/Common.glsl"
#include "Data/Shaders/Includes/Math.glsl"
#define FOG 
//--------------------------------------------------------------------------------------
// DEFINES
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// defined in common.glsl
// vec4 FOG_COLOR; 
// float FOG_NEAR_PLANE;
// float FOG_FAR_PLANE;
// float FOG_NEAR_FACTOR;
// float FOG_FAR_FACTOR;

//--------------------------------------------------------------------------------------
// FUNCTIONS
//--------------------------------------------------------------------------------------

vec4 ApplyFog( vec4 color, float viewDepth )
{
   #if defined(FOG)
      float fogFactor = smoothstep( FOG_NEAR_PLANE, FOG_FAR_PLANE, viewDepth ); 
      fogFactor = FOG_NEAR_FACTOR + (FOG_FAR_FACTOR - FOG_NEAR_FACTOR) * fogFactor; 
      //float fogFactor = RangeMap( viewDepth, FOG_NEAR_PLANE, FOG_FAR_PLANE, FOG_NEAR_FACTOR, FOG_FAR_FACTOR );
      color = mix( color, FOG_COLOR, fogFactor ); 
   #endif

   return color; 
}