float4x4 WorldViewProj;
float4x4 WorldMatrix;
float3 cameraPos;
float3 gLightDirection;
float3 FogRange; // x = start, y = invRange, z = opacity
float4 MainLightDiffuse;
float4 MainLightSpecular;
float4 EyePosition;

void vs_main(
     float4 position    : POSITION,
     float3 normal      : NORMAL, 
     float3 tangent     : TANGENT,
     float3 binormal    : BINORMAL,
     float2 texcoords   : TEXCOORD0,
 out float4 oPosition   : POSITION,
 out float2 oTexcoords  : TEXCOORD0,
 out float3 oHalfVector : TEXCOORD1,
 out float3 oLightDir   : TEXCOORD2,
 out float4 oNormalFog  : TEXCOORD3,  // world space
 out float4 oDiffuse    : COLOR0,
 out float4 oSpecular   : COLOR1
 )
{
    float4 mat_specular = float4(0.4f, 0.4f, 0.4f, 1.0f);
    float4 mat_diffuse = float4(0.9f, 0.9f, 0.9f, 1.0f);
    
    float4 light_diffuse = MainLightDiffuse;
    float4 light_specular = MainLightSpecular;
    

    float3 worldPos = mul(WorldMatrix, position).xyz;
    float3 lightDir = -gLightDirection;
    float3 viewDir = cameraPos - worldPos;
    float3 halfVector = normalize(normalize(lightDir) + normalize(viewDir));
  
    float3x3 tbnMatrix = float3x3(tangent, binormal , normal);
                                  

    oPosition = mul(WorldViewProj, position);
    oTexcoords = texcoords;
    oHalfVector = mul(tbnMatrix, halfVector);
    oLightDir = mul(tbnMatrix, lightDir);
    oDiffuse = mat_diffuse * light_diffuse;
    oSpecular = mat_specular * light_specular;
    
    float fog = FogRange.z * clamp(FogRange.y * (length(worldPos - EyePosition) - FogRange.x), 0.0, 1.0);
    oNormalFog = float4(normalize(normal), fog);
}
