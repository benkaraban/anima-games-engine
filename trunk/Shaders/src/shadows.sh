#ifndef SHADOW_SH_
#define SHADOW_SH_

sampler2D SamplerStaticShadow;

float4 StaticShadowDiffusion; // xy = diffusion range, z = opacity, w = bias

#define SHADOW_MAP_SIZE 8192
//#define SHADOW_MAP_SIZE 4096

float staticShadowOpacity1(float2 tcoords, float lightZ)
{
    float StaticShadowBias = StaticShadowDiffusion.w;
    
    float nearestZ = tex2D(SamplerStaticShadow, tcoords).x + StaticShadowBias;
    
    return (lightZ > nearestZ);
}

float staticShadowOpacityPCF(float2 tcoords, float lightZ)
{
    float2 unnormalized = tcoords * SHADOW_MAP_SIZE;
    float4 occluder1, occluder2, occluder3, occluder4;
    occluder1.x = staticShadowOpacity1((unnormalized + float2( -1.5f, -0.5f ))/ SHADOW_MAP_SIZE, lightZ);
    occluder1.y = staticShadowOpacity1((unnormalized + float2( -0.5f, -0.5f ))/ SHADOW_MAP_SIZE, lightZ);
    occluder1.z = staticShadowOpacity1((unnormalized + float2( -0.5f, -1.5f ))/ SHADOW_MAP_SIZE, lightZ);
    occluder1.w = staticShadowOpacity1((unnormalized + float2( -1.5f, -1.5f ))/ SHADOW_MAP_SIZE, lightZ);

    occluder2.x = staticShadowOpacity1((unnormalized + float2(  1.5f, -0.5f ))/ SHADOW_MAP_SIZE, lightZ);
    occluder2.y = staticShadowOpacity1((unnormalized + float2(  0.5f, -0.5f ))/ SHADOW_MAP_SIZE, lightZ);
    occluder2.z = staticShadowOpacity1((unnormalized + float2(  0.5f, -1.5f ))/ SHADOW_MAP_SIZE, lightZ);
    occluder2.w = staticShadowOpacity1((unnormalized + float2(  1.5f, -1.5f ))/ SHADOW_MAP_SIZE, lightZ);

    occluder3.x = staticShadowOpacity1((unnormalized + float2( -1.5f,  0.5f ))/ SHADOW_MAP_SIZE, lightZ);
    occluder3.y = staticShadowOpacity1((unnormalized + float2( -0.5f,  0.5f ))/ SHADOW_MAP_SIZE, lightZ);
    occluder3.z = staticShadowOpacity1((unnormalized + float2( -0.5f,  1.5f ))/ SHADOW_MAP_SIZE, lightZ);
    occluder3.w = staticShadowOpacity1((unnormalized + float2( -1.5f,  1.5f ))/ SHADOW_MAP_SIZE, lightZ);

    occluder4.x = staticShadowOpacity1((unnormalized + float2(  1.5f,  0.5f ))/ SHADOW_MAP_SIZE, lightZ);
    occluder4.y = staticShadowOpacity1((unnormalized + float2(  0.5f,  0.5f ))/ SHADOW_MAP_SIZE, lightZ);
    occluder4.z = staticShadowOpacity1((unnormalized + float2(  0.5f,  1.5f ))/ SHADOW_MAP_SIZE, lightZ);
    occluder4.w = staticShadowOpacity1((unnormalized + float2(  1.5f,  1.5f ))/ SHADOW_MAP_SIZE, lightZ);
    
    float4 occluder44 = float4(dot(occluder1, float4(0.25, 0.25, 0.25, 0.25)),
                               dot(occluder2, float4(0.25, 0.25, 0.25, 0.25)),
                               dot(occluder3, float4(0.25, 0.25, 0.25, 0.25)),
                               dot(occluder4, float4(0.25, 0.25, 0.25, 0.25)));
    
    return dot(occluder44, float4(0.25, 0.25, 0.25, 0.25));
}

#define DISP 0.001

float staticShadowOpacityBlur(float2 tcoords, float lightZ, float2 disp)
{
    float4 occluder1;
    occluder1.x = staticShadowOpacityPCF(tcoords + disp * float2(-1.0f, -1.0f), lightZ);
    occluder1.y = staticShadowOpacityPCF(tcoords + disp * float2( 1.0f, -1.0f), lightZ);
    occluder1.z = staticShadowOpacityPCF(tcoords + disp * float2( 1.0f,  1.0f), lightZ);
    occluder1.w = staticShadowOpacityPCF(tcoords + disp * float2(-1.0f,  1.0f), lightZ);
    
    float4 occluder2;
    occluder2.x = staticShadowOpacityPCF(tcoords + disp * float2( 0.0f, -1.0f), lightZ);
    occluder2.y = staticShadowOpacityPCF(tcoords + disp * float2( 0.0f,  1.0f), lightZ);
    occluder2.z = staticShadowOpacityPCF(tcoords + disp * float2( 1.0f,  0.0f), lightZ);
    occluder2.w = staticShadowOpacityPCF(tcoords + disp * float2(-1.0f,  0.0f), lightZ);
    
    float2 occ = float2(
        dot(occluder1, float4(0.25, 0.25, 0.25, 0.25)),
        dot(occluder2, float4(0.25, 0.25, 0.25, 0.25))
        );
    return dot(occ, float2(0.5, 0.5));
}

float staticShadowOpacityBlur4(float2 tcoords, float lightZ)
{
    float4 occluder;
    occluder.x = staticShadowOpacityBlur(tcoords, lightZ, 0.5 * StaticShadowDiffusion.xy);
    occluder.y = staticShadowOpacityBlur(tcoords, lightZ, 1.0 * StaticShadowDiffusion.xy);
    occluder.z = staticShadowOpacityBlur(tcoords, lightZ, 2.0 * StaticShadowDiffusion.xy);
    occluder.w = staticShadowOpacityBlur(tcoords, lightZ, 3.0 * StaticShadowDiffusion.xy);
    
    return dot(occluder, float4(0.25, 0.25, 0.25, 0.25));
}

#ifdef ULTRA_HQ_FOR_LIGHTMAP
#define SS_BLUR_KERNEL_SIZE 3
#else
#define SS_BLUR_KERNEL_SIZE 2
#endif

float staticShadowOpacityBlurK(float2 tcoords, float lightZ)
{
    float occ = 0.0f;
    
    for(int ii = -SS_BLUR_KERNEL_SIZE; ii <= SS_BLUR_KERNEL_SIZE; ii++)
    {
        float du = float(ii) / float(SS_BLUR_KERNEL_SIZE);
    
        for(int jj = -SS_BLUR_KERNEL_SIZE; jj <= SS_BLUR_KERNEL_SIZE; jj++)
        {
            float dv = float(jj) / float(SS_BLUR_KERNEL_SIZE);
            occ += staticShadowOpacityPCF(tcoords + StaticShadowDiffusion.xy * float2(du, dv), lightZ);
        }
    }
    
    occ /= float((2*SS_BLUR_KERNEL_SIZE + 1) * (2*SS_BLUR_KERNEL_SIZE + 1));
    
    return occ;
}

float staticShadowTerm(float4 lightPos)
{
    float StaticShadowOpacity = StaticShadowDiffusion.z;
    float2 shadowTex = 0.5 * lightPos.xy / lightPos.w + float2( 0.5, 0.5 );
    shadowTex.y = 1.0f - shadowTex.y;
   
    float lightZ = lightPos.z;
    
    float opacity = staticShadowOpacityBlurK(shadowTex, lightZ);
    return 1.0 - (opacity * StaticShadowOpacity);
}

#endif
