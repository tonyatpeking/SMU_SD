vec3 ComputeDiffuse( vec3 dirToLight, vec4 lightColor, vec3 fragNormal )
{
    float dot3 = dot(dirToLight, fragNormal);
    return max( dot3, 0 ) * lightColor.xyz;
}

vec3 ComputeSpecular( vec3 dirToLight, vec4 lightColor,
    vec3 camDir, vec3 fragNormal, float specAmount, float specPower )
{
    vec3 reflectDir = reflect( -dirToLight, fragNormal );
    float dotReflect = max( 0, dot( camDir, reflectDir ) );
    float specFactor = specAmount * pow( dotReflect, specPower );
    return specFactor * lightColor.xyz;
}

//from https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
float ComputeAttenuation( float lightDist, float sourceRadius )
{
    float temp = lightDist / sourceRadius + 1;
    return 1.0f / (temp * temp);
}

vec4 NormalToColor( vec3 normal )
{
    // maps -1,1 to 0,1
    return vec4( normal * 0.5 + 0.5, 1 );
}