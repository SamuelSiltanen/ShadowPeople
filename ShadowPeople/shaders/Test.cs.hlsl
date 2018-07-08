#include "Test.if.h"
#include "OctahedralNormal.h.hlsl"

float2 unpackUintToFloat2(uint i)
{
    return float2(i & 0xffff, i >> 16) / 65535.f;
}

static const float3 SunDir = float3(0.577f, 0.577f, -0.577f);

[numthreads(ThreadGroupsX, ThreadGroupsY, ThreadGroupsZ)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	if (any(DTid.xy >= size)) return;

	uint4 pixel     = gBuffer[DTid.xy];
    if (all(pixel != 0)) 
    {
        litBuffer[DTid.xy] = float4(0.f, 0.f, 0.f, 1.f); // Skip empty pixels
    }
    else
    {
        float2 uvFlt    = unpackUintToFloat2(pixel.x);
        float2 normFlt  = unpackUintToFloat2(pixel.y);

        float3 normal   = decodeOctahedral(normFlt);

        // Fake shading
        float3 color    = float3(uvFlt.x, uvFlt.y, 0.5f);
        color *= (0.2f + 0.8f * saturate(dot(normal, SunDir)));

	    litBuffer[DTid.xy] = float4(color, 1.f);
    }
}
