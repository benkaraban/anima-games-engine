#include <tools.sh>

float4x4 World;

float4x4 StaticLightViewProj;

float4 EyePosition;

float2 ViewportSize;

bool IsTextureBorderOn : register(b0);

#define EPSILON 0.01

void vs_main(
     float4 position    : POSITION,
     float3 normal      : NORMAL,
     float2 texcoords   : TEXCOORD0,
     float4 tangentBi   : TEXCOORD4,
     
 out float4 oClipPos    : POSITION,
 out float3 oWorldPos   : TEXCOORD0,
 out float3 oNormal     : TEXCOORD1,
 out float2 oTexcoords  : TEXCOORD2,
 
#if TSN_FLAG || DEBUG_FLAG
 out float3 oTangent    : TEXCOORD4,
 out float3 oBitangent  : TEXCOORD5,
#endif
 out float3 oEyeVec     : TEXCOORD6,
 out float4 oLightPosS  : TEXCOORD7
)
{
#if PACKED_NORMAL_FLAG
    normal = expand(normal);
#endif

    oTexcoords = texcoords;
    
    if(IsTextureBorderOn)
        texcoords *= (ViewportSize - float2(1.0 - EPSILON, 1.0 - EPSILON)) / ViewportSize;

    texcoords.y = 1.0 - texcoords.y;
    float2 pos = 2.0 * texcoords - float2(1.0, 1.0);
    oClipPos = float4(pos, 0.0, 1.0);
    
    if(!IsTextureBorderOn)
        oClipPos.xy += float2(-1.0, 1.0) / ViewportSize;

    oWorldPos = mul(World, position).xyz;
    oNormal = normalize(mul(World, float4(normal, 0.0)).xyz);

#if TSN_FLAG || DEBUG_FLAG
    oTangent = normalize(mul(World, float4(tangentBi.xyz, 0.0)).xyz);
    oBitangent = normalize(cross(oNormal, oTangent) * tangentBi.w);
#endif

    oEyeVec = EyePosition.xyz - oWorldPos;
    oLightPosS = mul(StaticLightViewProj, float4(oWorldPos, 1.0));
}
