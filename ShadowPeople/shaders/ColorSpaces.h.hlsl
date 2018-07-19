#ifndef SP_COLOR_SPACES_H_HLSL
#define SP_COLOR_SPACES_H_HLSL

float3 CIEXYZ2sRGB(float3 XYZ)
{
    const float3x3 mat =
    {
        3.2406, -1.5372, -0.4986,
        -0.9689, 1.8758, 0.0415,
        0.0557, -0.2040, 1.0570
    };
    return mul(mat, XYZ);
}

#endif
