#include "ShaderInterface.h"

BEGIN_SHADER_INTERFACE(GeometryRenderer)

CBuffer(constants,
{
    float4x4    view;
    float4x4    proj;
});

// TODO: Share this between CPU and GPU
struct Vertex
{
    float3  position;
    float3  normal;
    float2  uv;
};

StructuredBuffer<Vertex>    vertexBuffer;

THREAD_GROUP_SIZE(1, 1, 1)

END_SHADER_INTERFACE(GeometryRenderer)