
float3 expand(float3 v)
{
    return 2.0 * (v - 0.5);
}

float4 expand(float4 v)
{
    return 2.0 * (v - 0.5);
}

// This assumes NdotL comes clamped
float getFresnel(float NdotL, float fresnelBias, float fresnelPow)
{
    float facing = max(0.0001, (1.0 - NdotL));
    return  max(fresnelBias + (1.0-fresnelBias) * pow(facing, fresnelPow), 0.0);
}

float3 pack(float3 v)
{
    return 0.5 * (v + 1.0);
}

float3 getPixelNormal(sampler2D s, float2 texcoords, bool isDXT5)
{
    float4 n = tex2D(s, texcoords);
    n = 2.0 * (n - 0.5);

    if(isDXT5)
    {
        n.xy = n.ag;
        n.z = sqrt(1.0 - dot(n.xy, n.xy));
    }

    return normalize(n.xyz);
}

float luminance(float3 c)
{
    return dot(c, float3(0.299, 0.587, 0.114));
}

float3 getPixelNormal(sampler2D s, float2 texcoords)
{
#if NORMAL_MAP_DXT5_FLAG
    return getPixelNormal(s, texcoords, true);
#else
    return getPixelNormal(s, texcoords, false);
#endif
}
