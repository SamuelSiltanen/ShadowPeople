#include "GeometryRenderer.if.h"

struct VSOutput
{
    //float3  normal  : COLOR0;
    float4 orientation : COLOR0;
    float3  uv_bts      : COLOR1;
    float4  pos     : SV_POSITION;
};

VSOutput main(uint vertexId : SV_VertexID)
{
    Vertex v = vertexBuffer[vertexId];

    float4 worldPos = float4(v.position, 1.f);
    float4 viewPos  = mul(view, worldPos);
    float4 ndcPos   = mul(proj, viewPos);

    VSOutput output;

    //output.normal    = v.normal;
    output.orientation = v.orientation;
    output.uv_bts      = float3(v.uv, v.bitangentSign);
    output.pos         = ndcPos;

	return output;
}
