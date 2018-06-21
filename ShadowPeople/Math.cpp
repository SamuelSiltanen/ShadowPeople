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
}