#include "ShaderInterface.h"

BEGIN_SHADER_INTERFACE(Test2GS)

CBuffer(constants,
{
	float4x4	view;
	float4x4	proj;
});

THREAD_GROUP_SIZE(1, 1, 1)

END_SHADER_INTERFACE(Test2GS)
