#include "PatchRenderer.if.h"

struct VSInput
{
    uint    vertexId    : SV_VertexID;
    uint    patchId     : SV_InstanceID;
};

struct VSOutput
{
    float4  orientation : COLOR0;
    float3  uv_bts      : COLOR1;
    float4  pos         : SV_POSITION;
};

struct PatchInfo
{
    uint x;
    uint y;
    uint mip;
};

PatchInfo decodePatchInfo(Patch patch)
{
    PatchInfo info;

    info.x   = patch.id1 & 0xfffff;
    info.y   = ((patch.id2 & 0xff) << 12) | (patch.id1 >> 20);
    info.mip = patch.id2 >> 8;

    return info;
}

VSOutput main(VSInput input)
{
    uint index  = patchIndices[input.patchId];
    Patch patch = patchBuffer[index];

    PatchInfo info  = decodePatchInfo(patch);



    VSOutput output;

    output.orientation  = float4(0.f, 0.f, 0.f, 1.f);
    output.uv_bts       = float3(0.f, 0.f, 0.f);
    output.pos          = float4(0.f, 0.f, 0.f, 1.f);//ndcPos;

	return output;
}