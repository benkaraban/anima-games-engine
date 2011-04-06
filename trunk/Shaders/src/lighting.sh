#ifndef LIGHTING_SH_
#define LIGHTING_SH_

float3 MainLightDir;

float4 MainLightAmbient;
float4 MainLightDiffuse0;
float4 MainLightDiffuse1;
float4 MainLightDiffuse2;
float4 MainLightSpecular;

float2 Occlusion;

float3 PointLightPosition0;
float4 PointLightAmbient0;
float4 PointLightDiffuse0;
float4 PointLightSpecular0;
float  PointLightInvRadius0;

float3 PointLightPosition1;
float4 PointLightAmbient1;
float4 PointLightDiffuse1;
float4 PointLightSpecular1;
float  PointLightInvRadius1;

float3 PointLightPosition2;
float4 PointLightAmbient2;
float4 PointLightDiffuse2;
float4 PointLightSpecular2;
float  PointLightInvRadius2;

sampler2D SamplerLight;
sampler2D SamplerOcclusion;

float4 diffuseMainLight(float3 N, float staticShadow, float2 tcoords, float occlusion)
{
    N = normalize(N);
    float3 L = -normalize(MainLightDir);
    float NdotL = dot(N, L);
    
    float4 diffuse = (  MainLightDiffuse0 * max(0.0,  NdotL) +
                        MainLightDiffuse1 * (1.0 - abs(NdotL)) +
                        MainLightDiffuse2 * max(0.0, -NdotL));
   
#ifdef HQ_PREVIEW_FLAG
    
    #ifdef ULTRA_HQ_FOR_LIGHTMAP
        // MainLightAmbient.w sera appliqué à la lightmap obtenue
        float MainLightMulIntensity = occlusion;
    #else
        float MainLightMulIntensity = MainLightAmbient.w * occlusion;
    #endif
    
    return MainLightMulIntensity * (MainLightAmbient + staticShadow * diffuse);
#else
    return MainLightAmbient.w * (MainLightAmbient + diffuse);
#endif
}

struct LightContrib
{
    float4 diffuse;
    float4 specular;
};

LightContrib pointLightContrib(
    float3 normal,
    float3 position,
    float3 eye,
    float  matShininess,
    float3 lightPosition,
    float4 lightAmbient,
    float4 lightDiffuse,
    float4 lightSpecular,
    float  invRadius,
    float  occlusion)
{
   float3 lvec = lightPosition - position;
   float3 L = normalize(lvec);
   float3 R = reflect(-L, normal);
   float NdotL = dot(L, normal);
   float4 ldiffuse = (0.5 + 0.5 * NdotL) * lightDiffuse;
   
   float4 lspecular = pow(max(0.00001f, dot(R, eye)), matShininess) * lightSpecular;

   float attenuation = max(0.0, 1.0 - length(lvec) * invRadius);

   LightContrib result;
   result.diffuse = occlusion * attenuation * (ldiffuse + lightAmbient);
   result.specular = occlusion * attenuation * lspecular;
   
#ifdef ULTRA_HQ_FOR_LIGHTMAP
    // MainLightAmbient.w sera appliqué à la lightmap obtenue alors que ce n'est pas le cas normalement
    // donc on divise par MainLightAmbient.w pour compenser
    // (ça ne marche que si MainLightAmbient.w >= 1.0)
   result.diffuse /= MainLightAmbient.w;
#endif
   return result;
}

LightContrib lightingTerm(
    float3 normal, 
    float3 eye, 
    float3 position, 
    float4 matDiffuse, 
    float4 matSpecular, 
    float matShininess,
    float2 lightmapCoords = float2(0.5, 0.5),
    float staticShadow = 1.0)
{
//   float3 L = -MainLightDir;
   float3 R = reflect(MainLightDir, normal);
//   float NdotL = 0.5 * (1.0 + dot(L, normal));

#ifdef HQ_PREVIEW_FLAG
    float OccContrast = Occlusion.x;
    float OccLum = Occlusion.y;

    float occ = OccLum + tex2D(SamplerOcclusion, lightmapCoords).r;
    float occlusion = saturate((1.0 + OccContrast) * (occ - 0.5) + 0.5);
#else
    float occlusion = 1.0;
#endif

#if STATIC_PIXEL_LIGHTING_FLAG
   float4 ldiffuse = MainLightAmbient.w * tex2D(SamplerLight, lightmapCoords);
#else
    float4 ldiffuse = diffuseMainLight(normal, staticShadow, lightmapCoords, occlusion);
#endif

#ifdef HQ_PREVIEW_FLAG
   float4 lspecular = pow(max(0.00001f, dot(R, eye)), matShininess) * MainLightSpecular * staticShadow;
#else
   float4 lspecular = pow(max(0.00001f, dot(R, eye)), matShininess) * MainLightSpecular;
#endif

#if POINT_LIGHT_COUNT >= 1
   LightContrib contrib0 = pointLightContrib(
      normal,
      position,
      eye,
      matShininess,
      PointLightPosition0,
      PointLightAmbient0,
      PointLightDiffuse0,
      PointLightSpecular0,
      PointLightInvRadius0,
      occlusion);

    ldiffuse += contrib0.diffuse;
#if ADV_SPECULAR_FLAG
    lspecular += contrib0.specular;
#endif
#endif

#if POINT_LIGHT_COUNT >= 2
   LightContrib contrib1 = pointLightContrib(
      normal,
      position,
      eye,
      matShininess,
      PointLightPosition1,
      PointLightAmbient1,
      PointLightDiffuse1,
      PointLightSpecular1,
      PointLightInvRadius1,
      occlusion);

    ldiffuse += contrib1.diffuse;
#if ADV_SPECULAR_FLAG
    lspecular += contrib1.specular;
#endif
#endif

#if POINT_LIGHT_COUNT >= 3
   LightContrib contrib2 = pointLightContrib(
      normal,
      position,
      eye,
      matShininess,
      PointLightPosition2,
      PointLightAmbient2,
      PointLightDiffuse2,
      PointLightSpecular2,
      PointLightInvRadius2,
      occlusion);

    ldiffuse += contrib2.diffuse;
#if ADV_SPECULAR_FLAG
    lspecular += contrib2.specular;
#endif
#endif

   LightContrib result;
   result.diffuse = ldiffuse * matDiffuse;
   result.specular = lspecular * matSpecular;

   return result;
}

#endif
