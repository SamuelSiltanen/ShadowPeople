#include "Types.hpp"
#include "Errors.hpp"
#include <math.h>

static const float Epsilon = 1e-6f;

// Vectors

float3 cross(const float3& a, const float3& b)
{
	return {
		a[1] * b[2] - a[2] * b[1],
		a[2] * b[0] - a[0] * b[2],
		a[0] * b[1] - a[1] * b[0]
	};
}

float4 cross(const float4& a, const float4& b)
{
	return {
		a[1] * b[2] - a[2] * b[1],
		a[2] * b[0] - a[0] * b[2],
		a[0] * b[1] - a[1] * b[0],
		0.f
	};
}

// Matrices

Matrix4x4::Matrix4x4()
{
	memset(m_elements.data(), 0, sizeof(float) * 16);
	m_elements[0]	= 1.f;
	m_elements[5]	= 1.f;
	m_elements[10]	= 1.f;
	m_elements[15]	= 1.f;
}

Matrix4x4::Matrix4x4(float4 row0, float4 row1, float4 row2, float4 row3)
{
	memcpy(&m_elements[0], &row0, sizeof(float) * 4);
	memcpy(&m_elements[4], &row1, sizeof(float) * 4);
	memcpy(&m_elements[8], &row2, sizeof(float) * 4);
	memcpy(&m_elements[12], &row3, sizeof(float) * 4);
}

float& Matrix4x4::operator()(int row, int column)
{
	return m_elements[row * 4 + column];
}

const float& Matrix4x4::operator()(int row, int column) const
{
	return m_elements[row * 4 + column];
}

float4 Matrix4x4::operator*(float4 vec)
{
	float4 result({0.f, 0.f, 0.f, 0.f});
	for (uint32_t i = 0; i < 4; i++)
	{
		for (uint32_t j = 0; j < 4; j++)
		{
			result[i] += m_elements[i * 4 + j] * vec[j];
		}
	}
	return result;
}

Matrix4x4 operator*(const Matrix4x4& lhs, const Matrix4x4& rhs)
{
	Matrix4x4 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float dot = 0.f;
			for (int k = 0; k < 4; k++)
			{
				dot += lhs(i, k) * rhs(k, j);
			}
			result(i, j) = dot;
		}
	}
	return result;
}

// Quaternions

Quaternion::Quaternion() :
	m_rotation{ 0.f, 0.f, 0.f, 1.f }
{}

Quaternion::Quaternion(float4 rotation) :
	m_rotation{ rotation[0], rotation[1], rotation[2], rotation[3] }
{}

Quaternion::Quaternion(float4 axis, float angle)
{
	float s = sinf(0.5f * angle);
	float c = cosf(0.5f * angle);
	m_rotation[0] = axis[0] * s;
	m_rotation[1] = axis[1] * s;
	m_rotation[2] = axis[2] * s;
	m_rotation[3] = c;
}

bool Quaternion::operator==(const Quaternion& q) const
{
	return (q.m_rotation[0] == m_rotation[0]) && 
			(q.m_rotation[1] == m_rotation[1]) && 
			(q.m_rotation[2] == m_rotation[2]) && 
			(q.m_rotation[3] == m_rotation[3]);
}

Quaternion Quaternion::conjugate() const
{
	return Quaternion({ m_rotation[0], m_rotation[1], m_rotation[2], -m_rotation[3] });
}

float4 Quaternion::rotate(float4 position) const
{
	// This is more stable, although rotating directly with the quaternion,
	// could be more pricipled and even a little faster
	Matrix4x4 rot = toMatrix();
	float4 product = rot * position;
	return product;
}

// The source uses a column-major matrices, so the results must be transposed:
// https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
Matrix4x4 Quaternion::toMatrix() const
{
	Matrix4x4 mat;

	float qx2 = m_rotation[0] * m_rotation[0];
	float qy2 = m_rotation[1] * m_rotation[1];
	float qz2 = m_rotation[2] * m_rotation[2];

	mat(0, 0) = 1.f - 2.f * (qy2 + qz2);
	mat(1, 1) = 1.f - 2.f * (qx2 + qz2);
	mat(2, 2) = 1.f - 2.f * (qx2 + qy2);

	float qxy = m_rotation[0] * m_rotation[1];
	float qzw = m_rotation[2] * m_rotation[3];

	mat(0, 1) = 2.f * (qxy + qzw);
	mat(1, 0) = 2.f * (qxy - qzw);

	float qxz = m_rotation[0] * m_rotation[2];
	float qyw = m_rotation[1] * m_rotation[3];

	mat(0, 2) = 2.f * (qxz - qyw);
	mat(2, 0) = 2.f * (qxz + qyw);

	float qyz = m_rotation[1] * m_rotation[2];
	float qxw = m_rotation[0] * m_rotation[3];

	mat(1, 2) = 2.f * (qyz + qxw);
	mat(2, 1) = 2.f * (qyz - qxw);

	return mat;
}

std::string Quaternion::debugOutput() const
{
	std::string s("(");
	s += std::to_string(m_rotation[0]);
	s += ", ";
	s += std::to_string(m_rotation[1]);
	s += ", ";
	s += std::to_string(m_rotation[2]);
	s += ", ";
	s += std::to_string(m_rotation[3]);
	s += ")";
	return s;
}

Quaternion operator+(Quaternion a, const Quaternion& b)
{
	a.m_rotation[0] += b.m_rotation[0];
	a.m_rotation[1] += b.m_rotation[1];
	a.m_rotation[2] += b.m_rotation[2];
	a.m_rotation[3] += b.m_rotation[3];
	return a;
}

Quaternion operator-(Quaternion a, const Quaternion& b)
{
	a.m_rotation[0] -= b.m_rotation[0];
	a.m_rotation[1] -= b.m_rotation[1];
	a.m_rotation[2] -= b.m_rotation[2];
	a.m_rotation[3] -= b.m_rotation[3];
	return a;
}

Quaternion operator*(Quaternion a, const Quaternion& b)
{
	float3 av{ a.m_rotation[0], a.m_rotation[1], a.m_rotation[2] };
	float3 bv{ b.m_rotation[0], b.m_rotation[1], b.m_rotation[2] };
	float sa = a.m_rotation[3];
	float sb = b.m_rotation[3];

	float3 cp	= cross(av, bv);
	float3 v	= cp + sa * bv + sb * av;
	float dp	= av.dot(bv);
	float s		= sa * sb - dp;

	Quaternion product({v[0], v[1], v[2], s});

	return product;
}

float Quaternion::lengthSq()
{
	float len = 0.f;
	len += m_rotation[0] * m_rotation[0];
	len += m_rotation[1] * m_rotation[1];
	len += m_rotation[2] * m_rotation[2];
	len += m_rotation[3] * m_rotation[3];
	return len;
}
