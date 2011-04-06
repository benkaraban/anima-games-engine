#include <tools.sh>

float4x4 World;
float4x4 WorldViewProj;
float4x4 WorldInvTrans;
float4x4 ViewProj;

float4x4 LightViewProj0;
float4x4 LightViewProj1;

float4 EyePosition;
float3 FogRange; // x = start, y = invRange, z = opacity

float2 DiffuseMapSize;
float2 LightingMapSize;


float3 MainLightDir;
float4 MainLightAmbient;
float4 MainLightDiffuse;

float4 MatEmissive;
float4 MatDiffuse;


float  ZBias;

row_major float3x4 WorldBone[MAX_BONES];

void vs_main(
     float4 position    : POSITION,
     float2 texcoords   : TEXCOORD0,
     float3 normal      : NORMAL,
     
     
#ifdef SKIN_FLAG
     float4 bonesId     : TEXCOORD6,
     float4 bonesWeight : TEXCOORD7,
#endif

 out float4 oPosition   : POSITION,
 out float4 oTexcoords  : TEXCOORD0,
 out float4 oColor      : COLOR0,
 out float  oFog        : FOG
)
{
#if PACKED_NORMAL_FLAG
    normal = expand(normal);
#endif    

    float3 oNormal;

#ifdef SKIN_FLAG
    float3 oWorldPos;
    bonesWeight /= dot(bonesWeight, float4(1.0, 1.0, 0.0, 0.0));

    oWorldPos  = bonesWeight.x * mul(WorldBone[bonesId.x], position);
    oWorldPos += bonesWeight.y * mul(WorldBone[bonesId.y], position);

    oNormal  = bonesWeight.x * mul((float3x3)WorldBone[bonesId.x], normal);   
    oNormal += bonesWeight.y * mul((float3x3)WorldBone[bonesId.y], normal);   
    
    oPosition = mul(ViewProj, float4(oWorldPos, 1));

#else // NO SKIN ---

    float3 oWorldPos;
    oWorldPos = mul(World, position);
    oPosition = mul(WorldViewProj, position);
    oNormal = mul((float3x3)WorldInvTrans, normal);   

#endif // NO SKIN


    //oPosition.z = max(0.0, oPosition.z - ZBias);

    oFog = 1.0 - FogRange.z * clamp(FogRange.y * (length(oWorldPos.xyz - EyePosition.xyz) - FogRange.x), 0.0, 1.0);

    float3 L = -MainLightDir;
    float3 R = reflect(MainLightDir, oNormal);
    float NdotL = 0.5 * (1.0 + dot(L, oNormal));  
    float4 diffuse = (max(0.0, NdotL) * MainLightDiffuse + MainLightAmbient) * MatDiffuse;
    
    
    oTexcoords.xy = texcoords;
    oTexcoords.zw = texcoords;


#if TEXTURE_INTERNAL_BORDER_FLAG
    
    oTexcoords.xy *= (DiffuseMapSize  - float2(1.0, 1.0)) / DiffuseMapSize;
    oTexcoords.zw *= (LightingMapSize - float2(1.0, 1.0)) / LightingMapSize;

    oTexcoords.xy += float2(0.5, 0.5) / DiffuseMapSize;
    oTexcoords.zw += float2(0.5, 0.5) / LightingMapSize;
#endif

    oColor = MatEmissive + diffuse;
    oColor.a = MatDiffuse.a;
}

