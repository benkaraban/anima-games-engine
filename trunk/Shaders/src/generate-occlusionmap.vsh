#include <tools.sh>

#define EPSILON 0.01

float4x4 World;

float2 TextureSize;

bool IsTextureBorderOn : register(b0);

void vs_main(
     float4 position    : POSITION,
     float3 normal      : NORMAL,
     float2 texcoords   : TEXCOORD0,
     float4 bonesWeight : TEXCOORD7, // bonesWeight.r = occlusion
     
 out float4 oClipPos    : POSITION,
 out float  oOcclusion  : TEXCOORD0
)
{
#if PACKED_NORMAL_FLAG
    normal = expand(normal);
#endif

    if(IsTextureBorderOn)
        texcoords *= (TextureSize - float2(1.0 - EPSILON, 1.0 - EPSILON)) / TextureSize;

    texcoords.y = 1.0 - texcoords.y;
    float2 pos = 2.0 * texcoords - float2(1.0, 1.0);
    oClipPos = float4(pos, 0.0, 1.0);
    
    if(!IsTextureBorderOn)
        oClipPos.xy += float2(-1.0, 1.0) / TextureSize;

    oOcclusion = bonesWeight.r;
}
