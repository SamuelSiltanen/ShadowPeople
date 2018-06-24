#include "ShaderInterface.h"

BEGIN_SHADER_INTERFACE(TestCS)

CBuffer(constants,
{
	int2	size;
	uint2	__padding;
});

Texture2D<uint4>	gBuffer;
RWTexture2D<float4> litBuffer;

THREAD_GROUP_SIZE(16, 16, 1)

END_SHADER_INTERFACE(TestCS)
