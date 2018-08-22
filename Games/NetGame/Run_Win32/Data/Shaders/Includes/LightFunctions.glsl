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



//--------------------------------------------------------------------------------------
// return 1 if fully lit, 0 if should be fully in shadow (ignores light)
float GetShadowFactor( vec3 position, vec3 normal, Light light )
{
   float shadow = light.usesShadow;
   if (shadow == 0.0f) {
      return 1.0f; 
   }

   // so, we're lit, so we will use the shadow sampler
   float biasFactor = max( dot( light.direction, normal ), 0.0f ); 
   biasFactor = sqrt(1 - (biasFactor * biasFactor)); 
   position -= light.direction * biasFactor * .25f; 

   vec4 clipPos = SHADOW_MAP_VP * vec4(position, 1.0f);
   vec3 ndcPos = clipPos.xyz / clipPos.w; 

   // put from -1 to 1 range to 0 to 1 range
   ndcPos = (ndcPos + vec3(1)) * .5f;
   
   // can give this a "little" bias
   // treat every surface as "slightly" closer"
   // returns how many times I'm pass (GL_LESSEQUAL)
   float isLit = texture( gTexShadow, ndcPos ).r; 
   // float my_depth = ndcPos.z; 
   
   // use this to feathre shadows near the border
   float minUV = min( ndcPos.x, ndcPos.y ); 
   float maxUV = max( ndcPos.x, ndcPos.y ); 
   float blend = 1.0f - min( smoothstep(0.0f, .05f, minUV), smoothstep(1.0, .95, maxUV) ); 

   // step returns 0 if nearest_depth is less than my_depth, 1 otherwise.
   // if (nearest_depth) is less (closer) than my depth, that is shadow, so 0 -> shaded, 1 implies light
   // float isLit = step( my_depth, nearest_depth ); // 

   // scale by shadow amount
   return mix( light.usesShadow * isLit, 1.0f, blend );  
}