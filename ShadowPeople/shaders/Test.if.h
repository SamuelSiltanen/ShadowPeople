#include "ShaderInterface.h"

BEGIN_SHADER_INTERFACE(TestCS)

CBuffer(constants,
{
	uint2	size;
	uint2	__padding;
});

RWTexture2D<float4> backBuffer;

THREAD_GROUP_SIZE(16, 16, 1)

END_SHADER_INTERFACE(TestCS)
