#include "../cpugpu/ShaderInterface.h"
#include "../cpugpu/GeometryTypes.h"

BEGIN_SHADER_INTERFACE(LineRenderer)

CBuffer(constants,
{
    float4x4 view;
    float4x4 proj;
});

StructuredBuffer<Vertex>  vertexBuffer;

GRAPHICS_PIPELINE

END_SHADER_INTERFACE(LineRenderer)
