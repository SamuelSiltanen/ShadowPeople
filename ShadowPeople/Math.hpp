#pragma once

#include "Types.hpp"

namespace math
{
	constexpr float Pi = 3.141592653589793f;

	template<typename T>
	T divRoundUp(T a, T b)
	{
		return (a % b) ? (a / b + 1) : (a / b);
	}

	Matrix4x4 rotateAroundY(float angle);
	Matrix4x4 rotateAroundX(float angle);
	Matrix4x4 rotateAroundZ(float angle);
	Matrix4x4 rotationMatrix(float yaw, float pitch = 0.f, float roll = 0.f);

    float2 encodeOctahedral(float3 n);
    float3 decodeOctahedral(float2 f);
}
