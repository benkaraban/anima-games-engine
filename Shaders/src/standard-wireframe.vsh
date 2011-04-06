float4x4 World;
float4x4 WorldViewProj;
float4x4 ViewProj;

row_major float3x4 WorldBone[MAX_BONES];

void vs_main(
     float4 position    : POSITION,
     
#ifdef SKIN_FLAG
     float4 bonesId     : TEXCOORD6,
     float4 bonesWeight : TEXCOORD7,
#endif

 out float4 oPosition   : POSITION
)
{
#ifdef SKIN_FLAG
    float3 worldPos;
    bonesWeight /= (bonesWeight.x + bonesWeight.y + bonesWeight.z + bonesWeight.w);

    worldPos  = bonesWeight.x * mul(WorldBone[bonesId.x], position);
    worldPos += bonesWeight.y * mul(WorldBone[bonesId.y], position);
    worldPos += bonesWeight.z * mul(WorldBone[bonesId.z], position);
    worldPos += bonesWeight.w * mul(WorldBone[bonesId.w], position);

    oPosition = mul(ViewProj, float4(worldPos, 1));

#else // NO SKIN ---

    oPosition = mul(WorldViewProj, position);

#endif
}

