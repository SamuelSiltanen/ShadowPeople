/*
    Copyright 2018 Samuel Siltanen
    Math.hpp
*/

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

    // Better sign() function that works for all numeric types
    template<typename T>
    int sign(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    // This assumes power of 2 value, for other values it rounds downwards
    uint32_t log2(uint32_t value);

	Matrix4x4 rotateAroundY(float angle);
	Matrix4x4 rotateAroundX(float angle);
	Matrix4x4 rotateAroundZ(float angle);
	Matrix4x4 rotationMatrix(float yaw, float pitch = 0.f, float roll = 0.f);

    float2 encodeOctahedral(float3 n);
    float3 decodeOctahedral(float2 f);
}
