float4x4 WorldViewProj;

void vs_main(
     float4 position    : POSITION,
     float2 texcoords   : TEXCOORD0,

 out float4 oPosition   : POSITION,   // clip space
 out float3 oNormal     : TEXCOORD0
 )
{
    oNormal = normalize(position.xyz);    
    oPosition = mul(WorldViewProj, position);
    
    // De cette manière, oPosition.z / oPosition.w est toujours égal à 1 et le pixel
    // est toujours le plus au fond possible du z buffer.
    oPosition.w = 1.0000001*oPosition.z;
}
