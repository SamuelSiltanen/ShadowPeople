#include "../cpugpu/ShaderInterface.h"

BEGIN_SHADER_INTERFACE(LightingCS)

CBuffer(constants,
{
    float4x4    invView;
	int2	    size;
	uint2	    __padding;
});

Texture2D<uint4>	gBuffer;
RWTexture2D<float4> litBuffer;

Texture2D<float4>   albedoRoughness;
Texture2D<float2>   normal;
sampler             bilinearSampler;

THREAD_GROUP_SIZE(16, 16, 1)

END_SHADER_INTERFACE(LightingCS)
