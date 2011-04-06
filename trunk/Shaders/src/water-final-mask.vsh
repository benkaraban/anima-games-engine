float4x4 WorldViewProj;

void vs_main(
     float4 position      : POSITION,

 out float4 oPosition     : POSITION,   // clip space
 out float4 oColor        : COLOR0
 )
{
    oPosition = mul(WorldViewProj, position);
    oColor = float4(0.0, 0.0, 0.0, 1.0);
}
