#include "Test2.if.h"

struct VSOutput
{
	float4 color	: COLOR;
	float4 pos		: SV_POSITION;
};

static const uint indices[12 * 3] =
{
	0, 1, 2,
	2, 1, 3,
	2, 3, 6,
	6, 3, 7,
	0, 2, 6,
	6, 4, 0,
	6, 4, 7,
	7, 4, 5,
	4, 5, 0,
	0, 5, 1,
	1, 5, 3,
	3, 5, 7
};

static const float4 vertexPositions[8] =
{
	float4(-5.f, -5.f, -5.f, 1.f),
	float4(-5.f, 5.f, -5.f, 1.f),
	float4(5.f, -5.f, -5.f, 1.f),
	float4(5.f, 5.f, -5.f, 1.f),
	float4(-5.f, -5.f, 5.f, 1.f),
	float4(-5.f, 5.f, 5.f, 1.f),
	float4(5.f, -5.f, 5.f, 1.f),
	float4(5.f, 5.f, 5.f, 1.f),
};

static const float4 colors[6] =
{
	float4(1.f, 0.f, 0.f, 1.f),
	float4(0.f, 1.f, 0.f, 1.f),
	float4(0.f, 0.f, 1.f, 1.f),
	float4(1.f, 1.f, 0.f, 1.f),
	float4(0.f, 1.f, 1.f, 1.f),
	float4(1.f, 0.f, 1.f, 1.f)
};

VSOutput main(uint vertexId : SV_VertexID)
{
	uint index		= indices[vertexId];
	float4 worldPos = vertexPositions[index];

	float4 viewPos	= mul(view, worldPos);
	float4 ndcPos	= mul(proj, viewPos);

	VSOutput output;

	output.pos		= ndcPos;
	output.color	= colors[vertexId / 6];

	return output;
}
