#include "LineRenderer.if.h"

float4 main(float3 color : COLOR0) : SV_TARGET
{
	return float4(color, 1.0f);
}
