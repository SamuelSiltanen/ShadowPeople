#include "Test2.if.h"

struct VSOutput
{
	float4 color	: COLOR;
	float4 pos		: SV_POSITION;
};

static const float4 vertexPositions[4] =
{
	float4(-.5f, -.5f, 0.1f, 1.f),
	float4(-.5f, .5f, 0.1f, 1.f),
	float4(.5f, -.5f, 0.1f, 1.f),
	float4(.5f, .5f, 0.1f, 1.f)
};

VSOutput main(uint vertexId : SV_VertexID)
{
	VSOutput output;

	output.pos		= vertexPositions[vertexId];
	output.color	= float4(1.f, 1.f, 1.f, 1.f);

	return output;
}
