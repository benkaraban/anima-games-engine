float4x4 WorldViewProj;

void vs_main(
     float4 position    : POSITION,
     float4 texcoords   : TEXCOORD0,

 out float4 oPosition   : POSITION,   // clip space
 out float2 oTexcoords  : TEXCOORD0,
 out float2 oTexcoordsNorm  : TEXCOORD1
 )
{
    float4 vPos = mul(WorldViewProj, position);
   
    oPosition=vPos;
    oTexcoords = float2(texcoords.x, texcoords.y);
    oTexcoordsNorm = float2(texcoords.z, texcoords.w);
}
