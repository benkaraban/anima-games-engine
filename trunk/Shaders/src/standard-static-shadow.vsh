float4x4 WorldViewProj;

void vs_main(
     float4 position    : POSITION,
     
 out float4 oPosition   : POSITION,
 out float2 oDepth      : TEXCOORD0
)
{
    oPosition = mul(WorldViewProj, position);
    oDepth = oPosition.zw;
}

