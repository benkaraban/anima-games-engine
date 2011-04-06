float4x4 WorldViewProj;
float4x4 WorldView;

float  SpriteCount;  // Nombre total de sprites dans la texture
float  SpriteCountU; // Nombre de sprites sur une ligne
float2 SpriteUVSize; // dimensions d'un sprite dans l'espace UV

float3 FogRange; // x = start, y = invRange, z = opacity

void vs_main(
     float4 position    : POSITION,
     float3 texcoords   : TEXCOORD0,
     float4 color       : COLOR,
     
#if REFRACTION_FLAG
 out float4 oClipPos    : TEXCOORD1,
 out float  oEyeDist    : TEXCOORD2,
#endif
 out float4 oPosition   : POSITION,   // clip space
 out float2 oTexcoords  : TEXCOORD0,
 out float4 oColor      : COLOR0
)
{
    float4 viewPos = mul(WorldView, position);
    float eyeDist = length(viewPos);
    float fog = FogRange.z * clamp(FogRange.y * (eyeDist - FogRange.x), 0.0, 1.0);

    oPosition = mul(WorldViewProj, position);

#if SPRITE_FLAG
    float t = clamp(round(texcoords.z * SpriteCount - 0.5), 0.0, SpriteCount - 1.0);
    float2 steps = float2(floor(fmod(t, SpriteCountU)), floor(t / SpriteCountU));
    oTexcoords = (texcoords.xy + steps) * SpriteUVSize;
#else
    oTexcoords = texcoords.xy;
#endif

#if REFRACTION_FLAG
    oClipPos = oPosition;
    oClipPos.y = -oClipPos.y;
    oClipPos.xy = (oClipPos.xy + oClipPos.w) * 0.5;
    oClipPos.zw = float2(1.0, oPosition.w);

    oEyeDist = eyeDist;    
    oColor = color;
#else
    oColor = color;
    oColor.a *= (1.0 - fog);
#endif
}
