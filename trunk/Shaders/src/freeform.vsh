float3 MainLightDir;

float4x4 WorldViewProj;
float4x4 WorldView;

float3 EyePos;
float3 FogRange;

void vs_main(
     float4 position    : POSITION,
     float3 texcoords   : TEXCOORD0,
     float4 color       : COLOR,
     float4 glow        : TEXCOORD1,
     
#if REFRACTION_FLAG
 out float4 oClipPos    : TEXCOORD1,
 out float  oEyeDist    : TEXCOORD2,
#endif
 out float4 oPosition   : POSITION,   // clip space

 out float2 oTexcoords  : TEXCOORD0,
 out float4 oColor      : COLOR0
)
{
#ifdef WORLD_SPACE_FLAG
    float eyeDist = position.z; // Horrible hack for ChronoRage
    float fog = min(1.0, FogRange.y * (FogRange.x - eyeDist));
#else
    float4 viewPos = mul(WorldView, position);
    float eyeDist = length(viewPos);
    float fog = 1.0 - FogRange.z * clamp(FogRange.y * (eyeDist - FogRange.x), 0.0, 1.0);
#endif

    oPosition = mul(WorldViewProj, position);
    oTexcoords = texcoords.xy;

#if REFRACTION_FLAG
    oClipPos = oPosition;
    oClipPos.y = -oClipPos.y;
    oClipPos.xy = (oClipPos.xy + oClipPos.w) * 0.5;
    oClipPos.zw = float2(1.0, oPosition.w);

    oEyeDist = eyeDist;    
    oColor = color;
#else

#if GLOW_FLAG
    oColor = glow;
    oColor.a *= fog;

#elif LIGHT_FLAG

    float NdotL = 0.5 * (1.0 + dot(MainLightDir, texcoords));
    NdotL = NdotL * NdotL;
    oColor = color * NdotL * fog;
   
#else
    oColor = color;
    oColor.a *= fog;
#endif

#endif

}
