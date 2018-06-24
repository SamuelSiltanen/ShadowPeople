struct PSInput
{
	float4 color	: COLOR;
};

uint4 main(PSInput input) : SV_TARGET
{
	return asuint(input.color.rgba);
}
