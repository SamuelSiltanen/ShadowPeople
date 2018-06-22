#include "ShaderInterface.h"

BEGIN_SHADER_INTERFACE(ImGuiRendererGS)

CBuffer(constants,
{
	float4x4 proj;
});

Texture2D<float4>	fonts;
sampler				fontSampler;

THREAD_GROUP_SIZE(16, 16, 1)

END_SHADER_INTERFACE(ImGuiRendererGS)
