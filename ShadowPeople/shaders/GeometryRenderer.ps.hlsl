#include "GeometryRenderer.if.h"

struct PSInput
{
    float4 orientation : COLOR0;
    float3 uv_bts      : COLOR1;   
};

uint packFloat2ToUint(float2 f)
{
    return (uint(f.y * 0xffff) << 16) + uint(f.x * 0xffff);
}

uint4 main(PSInput input) : SV_TARGET
{
    float4 orientation  = normalize(input.orientation);
    float2 uv           = input.uv_bts.xy;
    float bitangentSign = input.uv_bts.z;

    // UV-derivatives
    float2 duvdx    = ddx(uv);
    float2 duvdy    = ddy(uv);
    float2 du       = float2(duvdx.x, duvdy.x);
    float2 dv       = float2(duvdx.y, duvdy.y);

    // Mip calculation
    const float Log2TextureSize = 11.f; // Currently: 2048
    float mip       = Log2TextureSize + log2(max(length(du), length(dv)));

    uint uvInt      = packFloat2ToUint(uv);
    uint orientation_xy    = packFloat2ToUint(0.5f + 0.5f * orientation.xy);
    uint orientation_zw    = packFloat2ToUint(0.5f + 0.5f * orientation.zw);

	return uint4(uvInt, orientation_xy, orientation_zw, asuint(bitangentSign));
}
