#include <tools.sh>

float4x4 World;
float4x4 WorldViewProj;
float4x4 WorldInvTrans;
float4x4 ViewProj;

float4x4 StaticLightViewProj;

float4x4 DynamicLightViewProj0;
float4x4 DynamicLightViewProj1;

float4 EyePosition;
float3 FogRange; // x = start, y = invRange, z = opacity

float2 DiffuseMapSize;
float2 LightingMapSize;

float  ZBias;

row_major float3x4 WorldBone[MAX_BONES];

void vs_main(
     float4 position    : POSITION,
     float2 texcoords   : TEXCOORD0,
     float3 normal      : NORMAL,
     
     
#if TSN_FLAG || DEBUG_FLAG
     float4 tangentBi   : TEXCOORD4,
 out float3 oTangent    : TEXCOORD4,
 out float3 oBitangent  : TEXCOORD5,
#endif

#ifdef SKIN_FLAG
     float4 bonesId     : TEXCOORD6,
     float4 bonesWeight : TEXCOORD7,
#else
     float2 lightcoords : TEXCOORD1,
#endif

#if REFRACTIONS_FLAG
 out float4 oClipPos    : TEXCOORD6,
#endif

#ifdef SHADOW_FLAG
 out float4 oLightPos0  : TEXCOORD6,
 out float4 oLightPos1  : TEXCOORD7,
#endif

#if HQ_PREVIEW_FLAG
 out float4 oLightPosS  : TEXCOORD8,
#endif

 out float4 oPosition   : POSITION,   // clip space
 out float4 oTexcoords  : TEXCOORD0,
 out float4 oNormalFog  : TEXCOORD1,  // world space
 out float3 oEyeVec     : TEXCOORD2,  // point to eye
 out float3 oWorldPos   : TEXCOORD3
)
{
#if PACKED_NORMAL_FLAG
    normal = expand(normal);
#endif    
    
    float3 oNormal;
#if TSN_FLAG || DEBUG_FLAG
#if PACKED_NORMAL_FLAG
    tangentBi = expand(tangentBi);
#endif    
    float3 tangent = tangentBi.xyz;
#endif
    
#ifdef SKIN_FLAG
    bonesWeight /= (bonesWeight.x + bonesWeight.y + bonesWeight.z + bonesWeight.w);

    oWorldPos  = bonesWeight.x * mul(WorldBone[bonesId.x], position);
    oWorldPos += bonesWeight.y * mul(WorldBone[bonesId.y], position);
    oWorldPos += bonesWeight.z * mul(WorldBone[bonesId.z], position);
    oWorldPos += bonesWeight.w * mul(WorldBone[bonesId.w], position);

    oNormal  = bonesWeight.x * mul((float3x3)WorldBone[bonesId.x], normal);   
    oNormal += bonesWeight.y * mul((float3x3)WorldBone[bonesId.y], normal);   
    oNormal += bonesWeight.z * mul((float3x3)WorldBone[bonesId.z], normal);   
    oNormal += bonesWeight.w * mul((float3x3)WorldBone[bonesId.w], normal);   

#if TSN_FLAG || DEBUG_FLAG
    oTangent  = bonesWeight.x * mul((float3x3)WorldBone[bonesId.x], tangent);
    oTangent += bonesWeight.y * mul((float3x3)WorldBone[bonesId.y], tangent);
    oTangent += bonesWeight.z * mul((float3x3)WorldBone[bonesId.z], tangent);
    oTangent += bonesWeight.w * mul((float3x3)WorldBone[bonesId.w], tangent);

    oBitangent = cross(oNormal, oTangent) * tangentBi.w;
#endif

    oPosition = mul(ViewProj, float4(oWorldPos, 1));

#else // NO SKIN ---

    oWorldPos = mul(World, position).xyz;
    oPosition = mul(WorldViewProj, position);
    oNormal = mul((float3x3)WorldInvTrans, normal);   

#if TSN_FLAG || DEBUG_FLAG
    oTangent = mul((float3x3)WorldInvTrans, tangent);
    oBitangent = cross(oNormal, oTangent) * tangentBi.w;
#endif

#endif // NO SKIN

    oPosition.z = oPosition.z - ZBias;

    float fog = FogRange.z * clamp(FogRange.y * (length(oWorldPos.xyz - EyePosition.xyz) - FogRange.x), 0.0, 1.0);

#ifdef SHADOW_FLAG
    oLightPos0 = mul(DynamicLightViewProj0, float4(oWorldPos, 1.0));
    oLightPos1 = mul(DynamicLightViewProj1, float4(oWorldPos, 1.0));
    oLightPos1.w *= 2.0;
    oLightPos0.w *= 2.0;
    oLightPos0.z = oPosition.z;
#endif

#if HQ_PREVIEW_FLAG
    oLightPosS = mul(StaticLightViewProj, float4(oWorldPos, 1.0));
#endif

    oTexcoords.xy = texcoords;
#ifdef SKIN_FLAG
    oTexcoords.zw = texcoords;
#else
    oTexcoords.zw = lightcoords;
#endif

#if TEXTURE_INTERNAL_BORDER_FLAG
    
    oTexcoords.xy *= (DiffuseMapSize  - float2(1.0, 1.0)) / DiffuseMapSize;
    oTexcoords.zw *= (LightingMapSize - float2(1.0, 1.0)) / LightingMapSize;

    oTexcoords.xy += float2(0.5, 0.5) / DiffuseMapSize;
    oTexcoords.zw += float2(0.5, 0.5) / LightingMapSize;
#endif

#if REFRACTIONS_FLAG
    oClipPos = oPosition;
    oClipPos.y = -oClipPos.y;
    oClipPos.xy = (oClipPos.xy + oClipPos.w) * 0.5;
    oClipPos.zw = float2(1.0, oPosition.w);
#endif

    oNormalFog = float4(oNormal, fog);
    oEyeVec = EyePosition.xyz - oWorldPos;
}

