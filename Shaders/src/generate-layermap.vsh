#include <tools.sh>

#define EPSILON 0.01

float4x4 World;

float2 TextureSize;

bool IsTextureBorderOn : register(b0);

void vs_main(
     float4 position    : POSITION,
     float3 normal      : NORMAL,
     float2 texcoords   : TEXCOORD0,
     float4 tangentBi   : TEXCOORD4,

 out float4 oClipPos    : POSITION,
 out float3 oWorldPos   : TEXCOORD0,

 out float3 oNormal     : TEXCOORD1,
 out float3 oTangent    : TEXCOORD4,
 out float3 oBitangent  : TEXCOORD5,

 out float2 oOrigTexc   : TEXCOORD2
)
{
#if PACKED_NORMAL_FLAG
    normal = expand(normal);
    tangentBi = expand(tangentBi);
#endif
    
    float3 tangent = tangentBi.xyz;
    
    oOrigTexc = texcoords;
    
    if(IsTextureBorderOn)
        texcoords *= (TextureSize - float2(1.0 - EPSILON, 1.0 - EPSILON)) / TextureSize;
        
    texcoords.y = 1.0 - texcoords.y;
    float2 pos = 2.0 * texcoords - float2(1.0, 1.0);
    oClipPos = float4(pos, 0.0, 1.0);

    if(!IsTextureBorderOn)
        oClipPos.xy += float2(-1.0, 1.0) / TextureSize;
    
    oWorldPos  = position.xyz;
    oNormal    = normalize(mul(World, float4(normal, 0.0)).xyz);
    oTangent   = normalize(mul(World, float4(tangent, 0.0)).xyz);
    oBitangent = normalize(cross(oNormal, oTangent) * tangentBi.w);
}
