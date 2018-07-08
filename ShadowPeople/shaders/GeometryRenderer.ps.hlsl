#include "GeometryRenderer.if.h"
#include "OctahedralNormal.h.hlsl"

struct PSInput
{
    float3 normal   : COLOR0;
    float2 uv       : COLOR1;
};

uint packFloat2ToUint(float2 f)
{
    return (uint(f.y * 0xffff) << 16) + uint(f.x * 0xffff);
}

uint4 main(PSInput input) : SV_TARGET
{
    float3 normal   = normalize(input.normal);
    float2 uv       = saturate(input.uv);

    float2 octaNorm = encodeOctahedral(normal);

    uint uvInt      = packFloat2ToUint(uv);
    uint normInt    = packFloat2ToUint(octaNorm);

	return uint4(uvInt, normInt, 0, 1);
}
