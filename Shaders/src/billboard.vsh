#include <tools.sh>

float4x4 World;
float4x4 ViewProj;
float3 EyePosition;
float3 AlignVector;
float PartScale;

float4x4 WorldView;
float4x4 Proj;

float  SpriteCount;  // Nombre total de sprites dans la texture
float  SpriteCountU; // Nombre de sprites sur une ligne
float2 SpriteUVSize; // dimensions d'un sprite dans l'espace UV

float Factor;

float3 FogRange; // x = start, y = invRange, z = opacity

void vs_main(
     float4 position    : POSITION,
     float3 texcoords   : TEXCOORD0,
     float4 color       : COLOR,
     
#if ALIGN_MODE >= 3
     float2 sizeAngle   : TEXCOORD1,
     float4 alignVec    : TEXCOORD2,
#else
     float3 sizeAngle   : TEXCOORD1,
#endif
     
    //--------------------------------------------------------------
#if SPRITE_SMOOTH_FLAG 
 out float4 oTexcoords  : TEXCOORD0,
 out float  oTexAlpha   : TEXCOORD3,
#else
 out float2 oTexcoords  : TEXCOORD0,
#endif

#if REFRACTION_FLAG
 out float4 oClipPos    : TEXCOORD1,
 out float  oEyeDist    : TEXCOORD2,
#endif

 out float4 oPosition   : POSITION,   // clip space
 out float4 oColor      : COLOR0
)
{
    float2 size = PartScale * sizeAngle.xy;
    
#if ALIGN_MODE == 0
    // ----------------------------------------------------
    // BILLBOARD_ALIGN_FACE_CAM
    // ----------------------------------------------------
    float angle = sizeAngle.z;

    float cosa = cos(angle);
    float sina = sin(angle);
    float2x2 rotMat = { cosa,  sina,
                        sina, -cosa };
    float2 xy2D = mul(rotMat, size * (texcoords.xy - 0.5));

    float4 center = mul(WorldView, position);
    float4 cornerPos = center + float4(xy2D, 0.0, 0.0);
    float eyeDist = length(center);
    float fog = FogRange.z * clamp(FogRange.y * (eyeDist - FogRange.x), 0.0, 1.0);
    oPosition = mul(Proj, cornerPos);
    
#else
    // ----------------------------------------------------
    // BILLBOARD_ALIGN_* on vector / dir
    // ----------------------------------------------------
    float4 center = mul(World, position);
    float3 eye = normalize(center.xyz - EyePosition);

#if ALIGN_MODE == 1
    // BILLBOARD_ALIGN_HORIZONTAL_ON_VEC
    float3 xVec = normalize(cross(AlignVector, eye));
    float3 yVec = AlignVector;
    
#elif ALIGN_MODE == 2
    // BILLBOARD_ALIGN_VERTICAL_ON_VEC
    float3 xVec = AlignVector;
    float3 yVec = normalize(cross(AlignVector, eye));
    
#elif ALIGN_MODE == 3
    // BILLBOARD_ALIGN_HORIZONTAL_ON_DIR
    float3 alignVector = expand(alignVec).xyz;
    float3 xVec = alignVector;
    float3 yVec = normalize(cross(alignVector, eye));

#elif ALIGN_MODE == 4
    // BILLBOARD_ALIGN_VERTICAL_ON_DIR
    float3 alignVector = expand(alignVec).xyz;
    float3 xVec = normalize(cross(alignVector, eye));
    float3 yVec = alignVector;
#endif
    
    float2 localUV = size * (texcoords.xy - 0.5);
    
    float4 cornerPos = center;
    cornerPos.xyz += localUV.x * xVec + localUV.y * yVec;
    
    float eyeDist = length(center);
    float fog = FogRange.z * clamp(FogRange.y * (eyeDist - FogRange.x), 0.0, 1.0);

    oPosition = mul(ViewProj, cornerPos);

#endif
    
#if SPRITE_FLAG

#if SPRITE_SMOOTH_FLAG
    //float n = texcoords.z * SpriteCount - 0.5;
    float n = texcoords.z * SpriteCount - 0.5;
    
    float t1 = clamp(floor(n), 0.0, SpriteCount - 1.0);
    float t2 = clamp(t1 + 1.0, 0.0, SpriteCount - 1.0);
    float2 steps1 = float2(floor(fmod(t1, SpriteCountU)), floor(t1 / SpriteCountU));
    float2 steps2 = float2(floor(fmod(t2, SpriteCountU)), floor(t2 / SpriteCountU));
    oTexcoords.xy = (texcoords.xy + steps1) * SpriteUVSize;
    oTexcoords.zw = (texcoords.xy + steps2) * SpriteUVSize;
    oTexAlpha = n - t1;
#else
    float t = clamp(round(texcoords.z * SpriteCount - 0.5), 0.0, SpriteCount - 1.0);
    float2 steps = float2(floor(fmod(t, SpriteCountU)), floor(t / SpriteCountU));
    oTexcoords = (texcoords.xy + steps) * SpriteUVSize;
#endif

#else // !SPRITE_FLAG
    oTexcoords = texcoords.xy;
#endif
    
#if REFRACTION_FLAG
    oClipPos = oPosition;
    oClipPos.y = -oClipPos.y;
    oClipPos.xy = (oClipPos.xy + oClipPos.w) * 0.5;
    oClipPos.zw = float2(1.0, oPosition.w);

    oEyeDist = eyeDist;    
#endif

    oColor = color;
    oColor.rgb *= Factor;
    
#if PREMUL_ALPHA_FLAG
    oColor.rgb *= (1.0 - fog);
#endif
    oColor.a *= (1.0 - fog);
}
