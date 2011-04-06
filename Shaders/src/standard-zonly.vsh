float4x4 World;
float4x4 WorldViewProj;
float4x4 ViewProj;

row_major float3x4 WorldBone[MAX_BONES];

void vs_main(
     float4 position    : POSITION,
     float2 texcoords   : TEXCOORD0,
     float3 normal      : NORMAL,
     
#ifdef SKIN_FLAG
     float4 bonesId     : TEXCOORD6,
     float4 bonesWeight : TEXCOORD7,
#endif

 out float4 oPosition   : POSITION,
 out float3 oWorldPos   : TEXCOORD0
)
{
#ifdef SKIN_FLAG
    bonesWeight /= (bonesWeight.x + bonesWeight.y + bonesWeight.z + bonesWeight.w);

    oWorldPos  = bonesWeight.x * mul(WorldBone[bonesId.x], position);
    oWorldPos += bonesWeight.y * mul(WorldBone[bonesId.y], position);
    oWorldPos += bonesWeight.z * mul(WorldBone[bonesId.z], position);
    oWorldPos += bonesWeight.w * mul(WorldBone[bonesId.w], position);

    oPosition = mul(ViewProj, float4(oWorldPos, 1));

#else // NO SKIN ---

    oWorldPos = mul(World, position);
    oPosition = mul(WorldViewProj, position);

#endif
}

