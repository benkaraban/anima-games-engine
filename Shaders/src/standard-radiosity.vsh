#include <tools.sh>

float4x4 WorldViewProj;

float2 DiffuseMapSize;
float2 LightingMapSize;


void vs_main(
     float4 position    : POSITION,
     float2 texcoords   : TEXCOORD0,
     float4 normalOcc   : NORMAL,
     
 out float4 oPosition   : POSITION,   // clip space
 out float4 oTexcoords  : TEXCOORD0
)
{
    oPosition = mul(WorldViewProj, position);

    oTexcoords.xy = texcoords;
    oTexcoords.zw = texcoords;

#if TEXTURE_INTERNAL_BORDER_FLAG

    oTexcoords.xy *= (DiffuseMapSize  - float2(1.0, 1.0)) / DiffuseMapSize;
    oTexcoords.zw *= (LightingMapSize - float2(1.0, 1.0)) / LightingMapSize;

    oTexcoords.xy += float2(0.5, 0.5) / DiffuseMapSize;
    oTexcoords.zw += float2(0.5, 0.5) / LightingMapSize;
#endif
}

