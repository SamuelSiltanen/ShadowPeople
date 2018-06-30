#include "ImGuiRenderer.if.h"

struct VSInput
{
    float2 pos : POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

PSInput main(VSInput input)
{
    PSInput output;

    output.pos = mul(proj, float4(input.pos.xy, 0.f, 1.f));
    output.col = input.col;
    output.uv  = input.uv;

    return output;
}
