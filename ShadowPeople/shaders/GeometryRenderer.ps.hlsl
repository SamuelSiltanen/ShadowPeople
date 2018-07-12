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
    float3 normal   = input.normal;
    float2 uv       = input.uv;

    // UV-derivatives
    float2 duvdx    = ddx(uv);
    float2 duvdy    = ddy(uv);
    float2 du       = float2(duvdx.x, duvdy.x);
    float2 dv       = float2(duvdx.y, duvdy.y);

    // Find surface tangent and bitangent
    float alpha = du.x / proj._m00;
    float beta  = du.y / proj._m11;

    float f = camNear / ((1.f - alpha) * normal.x + (1.f - beta) * normal.y + normal.z);

    float xt = (alpha * normal.z + (alpha - beta) * normal.y) * f;
    float yt = (beta * normal.z + (beta - alpha) * normal.x) * f;
    float zt = -(xt * normal.x + yt * normal.y) / normal.z;

    float3 tangent = normalize(float3(xt, yt, zt));

    // Mip calculation
    const float Log2TextureSize = 11.f; // Currently: 2048
    float mip       = Log2TextureSize + log2(max(length(du), length(dv)));

    float2 octaNorm = encodeOctahedral(normal);
    float2 octaTang = encodeOctahedral(tangent);

    uint uvInt      = packFloat2ToUint(uv);
    uint normInt    = packFloat2ToUint(octaNorm);
    uint tangInt    = packFloat2ToUint(octaTang);

	return uint4(uvInt, normInt, tangInt, asuint(mip));
}
