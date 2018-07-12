#include "Lighting.if.h"
#include "OctahedralNormal.h.hlsl"

float2 unpackUintToFloat2(uint i)
{
    return float2(i & 0xffff, i >> 16) / 65535.f;
}

static const float3 SunDir = float3(0.577f, 0.577f, 0.577f);

[numthreads(ThreadGroupsX, ThreadGroupsY, ThreadGroupsZ)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	if (any(DTid.xy >= size)) return;

	uint4 pixel     = gBuffer[DTid.xy];
    if (all(pixel == 0)) 
    {
        litBuffer[DTid.xy] = float4(0.5f, 0.5f, 0.5f, 1.f); // Skip empty pixels
    }
    else
    {
        float2 uvFlt    = unpackUintToFloat2(pixel.x);
        float2 normFlt  = unpackUintToFloat2(pixel.y);
        float2 tangFlt  = unpackUintToFloat2(pixel.z);
        float mip       = asfloat(pixel.w);

        float3 viewNormal   = decodeOctahedral(normFlt);
        float3 worldNormal  = mul(invView, float4(viewNormal, 0.f)).xyz;

        float3 viewTangent  = decodeOctahedral(tangFlt);
        float3 worldTangent = mul(invView, float4(viewTangent, 0.f)).xyz;

        float3 worldBitangent = cross(worldNormal, worldTangent);

        // Fake shading
        float4 texel = albedoRoughness.SampleLevel(bilinearSampler, uvFlt, 0);
        float2 texel2 = normal.SampleLevel(bilinearSampler, uvFlt, 0);

        float3 n = decodeOctahedral(texel2);

        float3x3 base;
        base._m00_m01_m02 = worldBitangent;
        base._m10_m11_m12 = worldNormal;
        base._m20_m21_m22 = worldTangent;

        float3 detailNorm = mul(base, n);

        float3 color = texel.rgb;
        color *= (0.2f + 0.8f * saturate(dot(detailNorm, SunDir)));        

	    litBuffer[DTid.xy] = float4(color, 1.f);
    }
}
