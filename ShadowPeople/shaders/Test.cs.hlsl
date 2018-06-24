#include "Test.if.h"

[numthreads(ThreadGroupsX, ThreadGroupsY, ThreadGroupsZ)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	if (any(DTid.xy >= size)) return;

	float4 color = asfloat(gBuffer[DTid.xy]);

	litBuffer[DTid.xy] = color;
}
