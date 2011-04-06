float4x4 WorldViewProj;
float4x4 WorldView;

float3 FogRange; // x = start, y = invRange, z = opacity
float2 DUV0;
float2 DUV1;
float2 DUV2;

void vs_main(
     float4 position    : POSITION,
     float2 texcoords   : TEXCOORD0,
     
 out float4 oPosition   : POSITION,   // clip space
 out float2 oTexcoords0 : TEXCOORD0,
 out float2 oTexcoords1 : TEXCOORD1,
 out float2 oTexcoords2 : TEXCOORD2,

 #if REFRACTION_FLAG
 out float4 oClipPos    : TEXCOORD3,
 out float  oEyeDist    : TEXCOORD4
#else
 out float  oFog        : TEXCOORD3
#endif
)
{
    float4 viewPos = mul(WorldView, position);
    float eyeDist = length(viewPos);

    oPosition = mul(WorldViewProj, position);
    oTexcoords0 = texcoords + DUV0;
    oTexcoords1 = texcoords + DUV1;
    oTexcoords2 = texcoords + DUV2;
 
#if REFRACTION_FLAG
    oClipPos = oPosition;
    oClipPos.y = -oClipPos.y;
    oClipPos.xy = (oClipPos.xy + oClipPos.w) * 0.5;
    oClipPos.zw = float2(1.0, oPosition.w);
    oEyeDist = eyeDist;    
#else
    oFog = 1.0 - FogRange.z * clamp(FogRange.y * (eyeDist - FogRange.x), 0.0, 1.0);
#endif
}
