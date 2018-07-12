#include "Math.hpp"

namespace math
{
	Matrix4x4 rotateAroundY(float angle)
	{
		Matrix4x4 mat;
		float s = sinf(angle);
		float c = cosf(angle);
		mat(0, 0) = c;
		mat(0, 2) = -s;
		mat(2, 0) = s;
		mat(2, 2) = c;
		return mat;
	}

	Matrix4x4 rotateAroundX(float angle)
	{
		Matrix4x4 mat;
		float s = sinf(angle);
		float c = cosf(angle);
		mat(1, 1) = c;
		mat(1, 2) = -s;
		mat(2, 1) = s;
		mat(2, 2) = c;
		return mat;
	}

	Matrix4x4 rotateAroundZ(float angle)
	{
		Matrix4x4 mat;
		float s = sinf(angle);
		float c = cosf(angle);
		mat(0, 0) = c;
		mat(0, 1) = -s;
		mat(1, 0) = s;
		mat(1, 1) = c;
		return mat;
	}

	Matrix4x4 rotationMatrix(float yaw, float pitch, float roll)
	{
		Matrix4x4 mat = rotateAroundY(yaw);
		if (pitch != 0.f) mat = rotateAroundX(pitch) * mat;
		if (roll != 0.f) mat = rotateAroundZ(roll) * mat;
		return mat;
	}

    float2 octaWrap(float2 v)
    {
        return { (1.f - abs(v[1])) * (v[0] >= 0.f ? 1.f : -1.f),
                 (1.f - abs(v[0])) * (v[1] >= 0.f ? 1.f : -1.f) };
    }

    float3 octaWrap(float3 v)
    {
        float2 v2 = octaWrap(float2{v[0], v[1]});
        return { v2[0], v2[1], v[2] };
    }

    float saturate(float a)
    {
        return std::min(0.f, std::max(1.f, a));
    }

    float2 encodeOctahedral(float3 n)
    {
        float len = (abs(n[0]) + abs(n[1]) + abs(n[2]));
        n /= len;        
        if (n[2] < 0.f) n = octaWrap(n);
        n *= 0.5f;
        n += 0.5f;
        return { n[0], n[1] };
    }

    float3 decodeOctahedral(float2 f)
    {
        f = f * 2.f - 1.f;
        float3 n{ f[0], f[1], 1.f - abs(f[0]) - abs(f[1]) };
        float t = saturate(-n[2]);
        n[0] += n[0] >= 0.f ? -t : t;
        n[1] += n[1] >= 0.f ? -t : t;
        return normalize(n);
    }
}