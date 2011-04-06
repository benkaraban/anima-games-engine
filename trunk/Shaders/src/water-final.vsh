float4x4 WorldViewProj;
float4x4 World;

float4 EyePosition;
float3 FogRange; // x = start, y = invRange, z = opacity

void vs_main(
     float4 position      : POSITION,
     float2 texcoords     : TEXCOORD0,

 out float4 oPosition     : POSITION,   // clip space
 out float3 oTexcoordsFog : TEXCOORD0,
 out float3 oWorldPos     : TEXCOORD1,
 out float3 oEye          : TEXCOORD2,   // point to eye
 out float4 oClipPos      : TEXCOORD3
 )
{
    oWorldPos = mul(World, position);
    oPosition = mul(WorldViewProj, position);
    oEye = (EyePosition - oWorldPos).xyz;
    oTexcoordsFog.xy = texcoords;
    oTexcoordsFog.z = FogRange.z * clamp(FogRange.y * (length(oEye) - FogRange.x), 0.0, 1.0);
    
    oClipPos = oPosition;
    oClipPos.y = -oClipPos.y;
    oClipPos.xy = (oClipPos.xy + oClipPos.w) * 0.5;
    oClipPos.zw = float2(1.0, oPosition.w);
}
