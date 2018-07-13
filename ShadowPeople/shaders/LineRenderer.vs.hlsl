#include "LineRenderer.if.h"
#include "Quaternion.h.hlsl"

struct VSOutput
{
    float3 color    : COLOR0;
    float4 pos      : SV_POSITION;
};

static const float3 axis_colors[3] =
{
    float3(1.f, 0.f, 0.f),
    float3(0.f, 1.f, 0.f),
    float3(0.f, 0.f, 1.f)
};

static const float GizmoSize = 0.1f;

VSOutput main(uint vertexId : SV_VertexID)
{
    uint index  = vertexId / 6;
    uint number = vertexId % 6;

    Vertex v    = vertexBuffer[index];
    float3 pos  = v.position + 0.01 * v.normal;
    
    if (number & 1)
    {
        float3 dir = qRot(v.orientation, axis_colors[number / 2]);
        pos += GizmoSize * dir;
    }
    
    float4 worldPos = float4(pos, 1.f);
    float4 viewPos  = mul(view, worldPos);
    float4 ndcPos   = mul(proj, viewPos);

    VSOutput output;

    output.pos      = ndcPos;
    output.color    = axis_colors[number / 2];

	return output;
}