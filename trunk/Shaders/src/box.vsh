float4x4 WorldViewProj;
float4 Color;

void vs_main(
     float4 position    : POSITION,
 out float4 oPosition   : POSITION,  // clip space
 out float4 oColor      : COLOR0
)
{
    oPosition = mul(WorldViewProj, position);
    oColor = Color;
}
