float4x4 World;
float4x4 WorldViewProj;
float4x4 WorldInvTrans;
float4x4 ViewProj;

float3   GlowExtent;

row_major float3x4 WorldBone[MAX_BONES];

void vs_main(
     float4 position    : POSITION,
     float2 texcoords   : TEXCOORD0,
     float3 normal      : NORMAL,
     
#ifdef SKIN_FLAG
     float4 bonesId     : TEXCOORD6,
     float4 bonesWeight : TEXCOORD7,
#endif

 out float4 oPosition   : POSITION,   // clip space
 out float2 oTexcoords  : TEXCOORD0
)
{
    float3 oNormal;
    
#ifdef SKIN_FLAG
    float3 oWorldPos;
    
    oWorldPos = bonesWeight.x * mul(WorldBone[bonesId.x], position);
    oNormal = bonesWeight.x * mul((float3x3)WorldBone[bonesId.x], normal);   

    oWorldPos += bonesWeight.y * mul(WorldBone[bonesId.y], position);
    oNormal += bonesWeight.y * mul((float3x3)WorldBone[bonesId.y], normal);   

    oWorldPos += bonesWeight.z * mul(WorldBone[bonesId.z], position);
    oNormal += bonesWeight.z * mul((float3x3)WorldBone[bonesId.z], normal);   

    oWorldPos += bonesWeight.w * mul(WorldBone[bonesId.w], position);
    oNormal += bonesWeight.w * mul((float3x3)WorldBone[bonesId.w], normal);   

    oWorldPos += GlowExtent * oNormal;
    oPosition = mul(ViewProj, float4(oWorldPos, 1));
#else
    position += float4(GlowExtent * normal, 0.0);
    oPosition = mul(WorldViewProj, position);
#endif

    oTexcoords = texcoords;
}
