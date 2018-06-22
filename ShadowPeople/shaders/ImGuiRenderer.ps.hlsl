#include "ImGuiRenderer.if.h"

struct PSInput
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 uv  : TEXCOORD0;
};

float4 main(PSInput input) : SV_Target
{
	float4 colorOut = input.col * fonts.Sample(fontSampler, input.uv);
	return colorOut;
}
