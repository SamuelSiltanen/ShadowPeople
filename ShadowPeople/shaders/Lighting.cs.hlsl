#include "Lighting.if.h"
#include "OctahedralNormal.h.hlsl"
#include "Quaternion.h.hlsl"
#include "SkyModel.h.hlsl"
#include "../cpugpu/Constants.h"

static const float SunElevation = 80.f * PI / 180.f;
static const float SunAzimuth   = 44.f * PI / 180.f;

static const float3 Ambient     = float3(0.2f, 0.3f, 0.4);
static const float3 SunDir      = float3(sin(SunAzimuth), sin(SunElevation), cos(SunAzimuth) * cos(SunElevation));
static const float SunIntensity = 10.f;
static const float3 SunColor    = float3(0.9f, 0.8f, 0.7f);
static const float SunDiscCos   = 0.999989f;
static const float F0Dielectric = 0.04f;

float2 unpackUintToFloat2(uint i)
{
    return float2(i & 0xffff, i >> 16) / 65535.f;
}

float3 diffuseLight(float3 N, float3 L)
{
    return saturate(dot(N, L));
}

float G1V(float d, float k)
{
    return 1.f / (d * (1.f - k) + k);
}

float3 specularLight(float3 N, float3 L, float3 V, float alpha, float F0)
{
    float3 H        = normalize(V + L);
    float dotNL     = saturate(dot(N, L));
    float dotNV     = saturate(dot(N, V));
    float dotNH     = saturate(dot(N, H));
    float dotLH     = saturate(dot(L, H));
    
    float alphaSqr  = alpha * alpha;
    float denom     = dotNH * dotNH * (alphaSqr - 1.f) + 1.f;
    float D         = alphaSqr / (PI * denom * denom);

    float dotLH5    = pow(1.f - dotLH, 5.f);
    float F         = F0 + (1.f - F0) * dotLH5;

    float k         = alpha * 0.5f;
    float G         = G1V(dotNL, k) * G1V(dotNV, k);

    return dotNL * D * F * G;
}

float3 worldPos(uint2 coords, float depth)
{
    float2 screenUV = (coords + 0.5f) / float2(size);
    screenUV.y      = 1.f - screenUV.y;
    float3 ndcPos   = float3(2.f * screenUV - 1.f, depth);
    float4 viewPos  = mul(invProj, float4(ndcPos, 1.f));
    viewPos         /= viewPos.w;
    float4 worldPos = mul(invView, viewPos);
    return worldPos.xyz;
}

[numthreads(ThreadGroupsX, ThreadGroupsY, ThreadGroupsZ)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	if (any(DTid.xy >= size)) return;

    float3 color = 0.f;
    float depth = zBuffer[DTid.xy];	
    if (depth == 1.f) 
    {
        // Hit the sky
        float3 pixelWorldPos    = worldPos(DTid.xy, 1.f);
        float3 cameraWorldPos   = mul(invView, float4(0.f, 0.f, 0.f, 1.f)).xyz;
        float3 viewDir          = normalize(pixelWorldPos - cameraWorldPos);
        float3 L                = normalize(SunDir);

        float dotVL = dot(viewDir, L);

        if (dotVL >= SunDiscCos)
        {
            color = SunColor * SunIntensity;
        }
        else
        {
            color = skyColor(viewDir, L);
        }

        uint patchIdx = DTid.x / 128;
        uint patchIdy = DTid.y / 128;
        uint patchId = /*1 + 4 + 16 + */(patchIdy << 3) + patchIdx;
        Patch patch = patchMetadata[patchId];
        float scale = float(patch.maxHeight - patch.minHeight) / 65535.f;

        uint value = heightMap[int3(DTid.xy, 3)];
        float grey = /*float(value & 65535) / 65535.f*/patch.cacheOffset / 256.f;
        color = float3(grey, grey, grey);
    }
    else
    {
        uint4 pixel     = gBuffer[DTid.xy];

        // Unpack g-buffer
        float2 uvFlt    = unpackUintToFloat2(pixel.x);
        float2 oxyFlt   = unpackUintToFloat2(pixel.y);
        float2 ozwFlt   = unpackUintToFloat2(pixel.z);
        float bitangentSign = asfloat(pixel.w);
        float4 orientation = float4(oxyFlt.x, oxyFlt.y, ozwFlt.x, ozwFlt.y) * 2.f - 1.f;

        // Read material
        float4 texel    = albedoRoughness.SampleLevel(bilinearSampler, uvFlt, 0);
        float2 texel2   = normal.SampleLevel(bilinearSampler, uvFlt, 0);

        // Interpret material data
        float3 n        = decodeOctahedral(texel2);
        n.y = -n.y;
        float3 detailNorm = qRot(orientation, n);

        float3 albedo   = texel.rgb;

        // Based on Phong default exponent 60 and https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
        float alpha     = rsqrt(max(30.f * texel.w - 1.f, 1e-3));

        // Find out pixel view position
        float3 pixelWorldPos    = worldPos(DTid.xy, depth);
        float3 cameraWorldPos   = mul(invView, float4(0.f, 0.f, 0.f, 1.f)).xyz;

        float3 viewDir  = normalize(cameraWorldPos - pixelWorldPos);
        
        float3 diff     = albedo * diffuseLight(detailNorm, SunDir);
        float3 spec     = specularLight(detailNorm, SunDir, viewDir, alpha, F0Dielectric);
        
        color    = albedo * Ambient + SunIntensity * SunColor * (diff + spec);
    }
    /*
    // Cheap tone mapping
    float lum = dot(color, float3(0.2126f, 0.7152f, 0.0722f));
    color *= 1.f / (1.f + lum);
    color = pow(color, 1.f / 2.2f);
    */
    litBuffer[DTid.xy] = float4(color, 1.f);
}
