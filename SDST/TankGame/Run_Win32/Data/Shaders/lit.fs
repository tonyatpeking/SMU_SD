#version 420 core

#include "Data/Shaders/Includes/Common.glsl"
#include "Data/Shaders/Includes/Fog.glsl"

// Textures
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexNormal;


// Attributes ============================================
in vec2 passUV;
in vec4 passColor;
in vec3 passWorldPos;
in vec3 passWorldNormal;
in vec3 passWorldTangent;
in vec3 passWorldBitangent;
in vec3 passViewSpacePos;

out vec4 outColor;

// Entry Point ===========================================
void main( void )
{

    // normalized window coords
    vec2 fragCoordNorm = gl_FragCoord.xy / WINDOW_SIZE;


    // Interpolation is linear, so normals become not normal
    // over a surface, so renormalize it.
    vec3 vertWorldNormal = normalize(passWorldNormal);
    vec3 vertWorldTangent = normalize(passWorldTangent);
    vec3 vertWorldBitangent = normalize(passWorldBitangent);

    mat3 surfaceToWorld = mat3( vertWorldTangent, vertWorldBitangent, vertWorldNormal );

    // Color of this surface
    vec4 surfaceColor = texture( gTexDiffuse, passUV ) * passColor;
    vec3 surfaceNormalColor = texture( gTexNormal, passUV ).xyz;

    vec3 fragNormal = normalize( surfaceNormalColor * vec3( 2.0f, 2.0f, 1.0f ) + vec3( -1.0f, -1.0f, 0.0f ) );
    vec3 fragWorldNormal = surfaceToWorld * fragNormal;

    // used in final lighting equation to compute
    // final color of output - calculated from the light
    vec3 diffuseLight = vec3(0); // How much light is hitting the surface
    vec3 specLight = vec3(0);  // How much light is reflected back

    // vectors we'll use
    vec3 camDir = normalize(CAMERA_POSITION - passWorldPos); // direction to the camera

    diffuseLight = AMBIENT.xyz * AMBIENT.w;



for ( int lightIdx = 0; lightIdx < MAX_LIGHTS; ++lightIdx)
{
    vec3 dirPointLight = LIGHTS[lightIdx].position - passWorldPos;
    vec3 dirDirectionalLight = -LIGHTS[lightIdx].direction;
    vec3 dirToLight = normalize( mix( dirDirectionalLight, dirPointLight, LIGHTS[lightIdx].isPointLight )); // direction to light
    float lightDist = length(LIGHTS[lightIdx].position - passWorldPos);
    float attenuation = ComputeAttenuation(lightDist, LIGHTS[lightIdx].sourceRadius)
        * LIGHTS[lightIdx].color.w;
    float coneDot = dot( dirToLight, dirDirectionalLight );
    float spotlightDropoff = smoothstep( LIGHTS[lightIdx].coneOuterDot, LIGHTS[lightIdx].coneInnerDot, coneDot );

    diffuseLight += ComputeDiffuse( dirToLight, LIGHTS[lightIdx].color, fragWorldNormal )
        * attenuation * spotlightDropoff;

    specLight += ComputeSpecular( dirToLight, LIGHTS[lightIdx].color, camDir, fragWorldNormal,
        SPECULAR_AMOUNT, SPECULAR_POWER ) * attenuation * spotlightDropoff;
}


    // Surface lighting should never blow out surface. It is either fully lit or not
    diffuseLight = clamp(diffuseLight, vec3(0), vec3(1));

    // Reflected light on the other hand CAN exceed one (used for bloom)

    // Calculate final color; note, it should not change the alpha... so...
    // Alpha of one for surface light as it is multiplied in,
    // and 0 for specular since it is added in.
    vec4 finalColor = vec4(diffuseLight, 1) * surfaceColor + vec4(specLight, 0);
    // Reclamp so that we're in a valid color range.  May want to save off overflow
    // if doing bloom.
    finalColor = clamp(finalColor, vec4(0), vec4(1) );

    outColor = finalColor;

    
    outColor = ApplyFog( outColor, passViewSpacePos.z );

}