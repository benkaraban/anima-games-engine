float4x4 WorldViewProj;

void vertexFn(float4 position   : POSITION,
              float3 normal     : NORMAL,
              float2 texcoord   : TEXCOORD0,
          out float4 oPosition  : POSITION,
          out float2 oTexcoord  : TEXCOORD0,
          out float3 oNormal    : TEXCOORD1)
{
    oPosition = mul(WorldViewProj, position);
    oTexcoord = texcoord;
    oNormal = normal;
}
