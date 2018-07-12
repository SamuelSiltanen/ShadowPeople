#include "GeometryRenderer.if.h"

struct VSOutput
{
    float3  normal  : COLOR0;
    float2  uv      : COLOR1;
    float4  pos     : SV_POSITION;
};

VSOutput main(uint vertexId : SV_VertexID)
{
    Vertex v = vertexBuffer[vertexId];

    float4 worldPos = float4(v.position, 1.f);
    float4 viewPos  = mul(view, worldPos);
    float4 ndcPos   = mul(proj, viewPos);

    float3 viewNorm = mul(view, float4(v.normal, 0.f)).xyz;

    VSOutput output;

    output.normal   = viewNorm;
    output.uv       = v.uv;
    output.pos      = ndcPos;

	return output;
}
