float4x4 WorldViewProj;
float4x4 WorldView;

float3 FogRange; // x = start, y = invRange, z = opacity


float4 WindDirU;
float4 WindDirV;
float4 WindWaveLength; // Inclue la conversion => -PI / PI (= waveLength / 2PI)
float4 WindPhase;      // Inclue la phase temporelle => phase + 2PI * (ElapsedTime / period )
float4 WindIntensity;

float3 getWind(float3 position)
{
    float4 s = position.xxxx * WindDirU + position.zzzz * WindDirV;
    float4 t = s / WindWaveLength + WindPhase;
    float4 I = WindIntensity * sin(t);
    float dx = dot(I, WindDirU);
    float dz = dot(I, WindDirV);
    return float3(dx, 0.0, dz);
}

void vs_main(
     float4 position    : POSITION,
     float2 texcoords   : TEXCOORD0,
     float3 centerpos   : TEXCOORD1,
     
 out float4 oPosition   : POSITION,   // clip space
 out float2 oTexcoords  : TEXCOORD0,
 out float3 oTexcoords2 : TEXCOORD1,
 out float4 oColor      : COLOR0,
 out float  oFog        : FOG
)
{
    position.xyz += getWind(centerpos) * (1.0 - texcoords.y);
    
    float4 viewPos = mul(WorldView, position);
    oFog = 1.0 - FogRange.z * clamp(FogRange.y * (length(viewPos) - FogRange.x), 0.0, 1.0);

    oPosition = mul(WorldViewProj, position);
    oTexcoords = texcoords;
}
