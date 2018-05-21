#include "Test.if.h"

[numthreads(ThreadGroupsX, ThreadGroupsY, ThreadGroupsZ)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//if (any(DTid.xy >= size)) return;

	uint2 square			= DTid.xy >> 3;
	float4 checkerPattern	= ((square.x & 1) == (square.y & 1)) ?
		float4(1.f, 0.f, 0.5f, 1.f) :
		float4(0.f, 1.f, 0.5f, 1.f);

	backBuffer[DTid.xy] = checkerPattern;
}
