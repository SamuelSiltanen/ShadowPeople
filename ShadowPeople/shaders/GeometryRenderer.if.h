#include "../cpugpu/ShaderInterface.h"
#include "../cpugpu/GeometryTypes.h"

BEGIN_SHADER_INTERFACE(GeometryRenderer)

CBuffer(constants,
{
    float4x4    view;
    float4x4    proj;
    float       camNear;
    float3      __padding;
});

StructuredBuffer<Vertex>    vertexBuffer;

GRAPHICS_PIPELINE

END_SHADER_INTERFACE(GeometryRenderer)