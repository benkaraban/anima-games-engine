float4x4 WorldViewProj;
float4x4 World;
float4 EyePosition;
float3 FogRange; // x = start, y = invRange, z = opacity

float4 WaterDiffuse;

void vs_main(
     float4 position      : POSITION,
     float2 texcoords     : TEXCOORD0,

 out float4 oPosition     : POSITION,   // clip space
 out float2 oTexcoords    : TEXCOORD0,
 out float4 oColor        : COLOR0,
 out float  oFog          : FOG
 )
{
    oPosition = mul(WorldViewProj, position);
    oTexcoords = texcoords;
    float3 worldPos = mul(World, position).xyz;
    float eye = (EyePosition - worldPos).xyz;
    oFog = 1.0 - FogRange.z * clamp(FogRange.y * (length(eye) - FogRange.x), 0.0, 1.0);
    oColor = WaterDiffuse;
}
