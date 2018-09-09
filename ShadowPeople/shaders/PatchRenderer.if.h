#include "../cpugpu/ShaderInterface.h"
#include "../cpugpu/GeometryTypes.h"

BEGIN_SHADER_INTERFACE(PatchRenderer)

CBuffer(constants,
{
    float4x4    view;
    float4x4    proj;
});

Buffer<uint>            patchIndices;
StructuredBuffer<Patch> patchBuffer;

GRAPHICS_PIPELINE

END_SHADER_INTERFACE(PatchRenderer)
