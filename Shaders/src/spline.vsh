float4 Color;
float4x4 ViewProj;

void vs_main(
     float4 position    : POSITION,
 out float4 oPosition   : POSITION,   // clip space
 out float4 oColor      : COLOR0
)
{
    oPosition = mul(ViewProj, position);
    oColor = Color;
}
