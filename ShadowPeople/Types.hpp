/*
    Copyright 2018 Samuel Siltanen
    Types.hpp
*/

#pragma once

#include <array>
#include <vector>
#include <string>
#include <memory>

template<int N>
class BooleanVector
{
public:
	BooleanVector(bool fillValue = false)
	{
		for (int i = 0; i < N; i++)
		{
			m_elements[i] = fillValue;
		}
	}

	BooleanVector(std::initializer_list<bool> elements)
	{
		if (elements.size() <= N)
		{
			int i = 0;
			for (; i < static_cast<int>(elements.size()); i++)
			{
				m_elements[i] = elements.begin()[i];
			}
			for (; i < N; i++)
			{
				m_elements[i] = 0;
			}
		}
		else
		{
			for (int i = 0; i < N; i++)
			{
				m_elements[i] = elements.begin()[i];
			}
		}
	}

	BooleanVector operator==(const BooleanVector& rhs)
	{
		BooleanVector b(false);
		for (int i = 0; i < N; i++)
		{
			if (m_elements[i] == rhs.m_elements[i]) b[i] = true;
		}
		return b;
	}

	BooleanVector operator!=(const BooleanVector& rhs)
	{
		BooleanVector b(false);
		for (int i = 0; i < N; i++)
		{
			if (m_elements[i] != rhs.m_elements[i]) b[i] = true;
		}
		return b;
	}

	BooleanVector operator<(const BooleanVector& rhs)
	{
		BooleanVector b(false);
		for (int i = 0; i < N; i++)
		{
			if (m_elements[i] < rhs.m_elements[i]) b[i] = true;
		}
		return b;
	}

	BooleanVector operator<=(const BooleanVector& rhs)
	{
		BooleanVector b(false);
		for (int i = 0; i < N; i++)
		{
			if (m_elements[i] <= rhs.m_elements[i]) b[i] = true;
		}
		return b;
	}

	BooleanVector operator>(const BooleanVector& rhs)
	{
		BooleanVector b(false);
		for (int i = 0; i < N; i++)
		{
			if (m_elements[i] > rhs.m_elements[i]) b[i] = true;
		}
		return b;
	}

	BooleanVector operator>=(const BooleanVector& rhs)
	{
		BooleanVector b(false);
		for (int i = 0; i < N; i++)
		{
			if (m_elements[i] >= rhs.m_elements[i]) b[i] = true;
		}
		return b;
	}

	bool& operator[](const int i)
	{
		return m_elements[i];
	}

	const bool& operator[](const int i) const
	{
		return m_elements[i];
	}

	friend bool any(const BooleanVector& b)
	{
		for (int i = 0; i < N; i++)
		{
			if (b.m_elements[i]) return true;
		}
		return false;
	}

	friend bool all(const BooleanVector& b)
	{
		for (int i = 0; i < N; i++)
		{
			if (!b.m_elements[i]) return false;
		}
		return true;
	}
private:
	std::array<bool, N>	m_elements;
};

template<typename T, int N>
class ArithmeticVector
{
public:
	ArithmeticVector()
	{
		for (int i = 0; i < N; i++)
		{
			m_elements[i] = 0;
		}
	}

	ArithmeticVector(T fillValue)
	{
		for (int i = 0; i < N; i++)
		{
			m_elements[i] = fillValue;
		}
	}

	ArithmeticVector(std::initializer_list<T> elements)
	{
		if (elements.size() <= N)
		{
			int i = 0;
			for (; i < static_cast<int>(elements.size()); i++)
			{
				m_elements[i] = elements.begin()[i];
			}
			for (; i < N; i++)
			{
				m_elements[i] = 0;
			}
		}
		else
		{
			for (int i = 0; i < N; i++)
			{
				m_elements[i] = elements.begin()[i];
			}
		}
	}

	std::string debugOutput() const
	{
		std::string s("(");
		for (int i = 0; i < N; i++)
		{
			s += std::to_string(m_elements[i]);
			if (i < N - 1) s += ", ";
		}
		s += ")";
		return s;
	}

	/*
	ArithmeticVector(const ArithmeticVector&) = default;
	ArithmeticVector(const ArithmeticVector&&) = default;

	ArithmeticVector& operator=(const ArithmeticVector&) = default;
	ArithmeticVector& operator=(const ArithmeticVector&&) = default;
	*/
	bool operator==(const ArithmeticVector& rhs) const
	{
		for (int i = 0; i < N; i++)
		{
			if (m_elements[i] != rhs.m_elements[i]) return false;
		}
		return true;
	}

	T& operator[](const int i)
	{
		return m_elements[i];
	}

	const T& operator[](const int i) const
	{
		return m_elements[i];
	}

	float dot(const ArithmeticVector& a) const
	{
		float sum = 0.f;
		for (int i = 0; i < N; i++)
		{
			sum += m_elements[i] * a.m_elements[i];
		}
		return sum;
	}

	float length() const
	{
		float sum = 0.f;
		for (int i = 0; i < N; i++)
		{
			sum += m_elements[i] * m_elements[i];
		}
		return sqrtf(sum);
	}

	ArithmeticVector& operator+=(const ArithmeticVector& a)
	{
		for (int i = 0; i < N; i++)
		{
			m_elements[i] += a.m_elements[i];
		}
		return *this;
	}

	ArithmeticVector& operator-=(const ArithmeticVector& a)
	{
		for (int i = 0; i < N; i++)
		{
			m_elements[i] -= a.m_elements[i];
		}
		return *this;
	}

    ArithmeticVector& operator*=(const ArithmeticVector& a)
	{
		for (int i = 0; i < N; i++)
		{
			m_elements[i] *= a.m_elements[i];
		}
		return *this;
	}

	ArithmeticVector& operator/=(const ArithmeticVector& a)
	{
		for (int i = 0; i < N; i++)
		{
			m_elements[i] /= a.m_elements[i];
		}
		return *this;
	}

	BooleanVector<N> operator<(const ArithmeticVector& a)
	{
		BooleanVector<N> b(false);
		for (int i = 0; i < N; i++)
		{
			if (m_elements[i] < a.m_elements[i]) b[i] = true;
		}
		return b;
	}

	friend ArithmeticVector normalize(const ArithmeticVector& a)
	{
		return a / a.length();
	}

	// Binary operations
	friend ArithmeticVector operator+(ArithmeticVector lhs, const ArithmeticVector& rhs)
	{
		for (int i = 0; i < N; i++)
		{
			lhs.m_elements[i] += rhs.m_elements[i];
		}
		return lhs;
	}

	friend ArithmeticVector operator-(ArithmeticVector lhs, const ArithmeticVector& rhs)
	{
		for (int i = 0; i < N; i++)
		{
			lhs.m_elements[i] -= rhs.m_elements[i];
		}
		return lhs;
	}

	friend const ArithmeticVector operator*(ArithmeticVector lhs, const ArithmeticVector& rhs)
	{
		for (int i = 0; i < N; i++)
		{
			lhs.m_elements[i] *= rhs.m_elements[i];
		}
		return lhs;
	}

	friend const ArithmeticVector operator/(ArithmeticVector lhs, const ArithmeticVector& rhs)
	{
		for (int i = 0; i < N; i++)
		{
			lhs.m_elements[i] /= rhs.m_elements[i];
		}
		return lhs;
	}
private:
	std::array<T, N>	m_elements;
};

// Common specializations
using int2 = ArithmeticVector<int, 2>;
using int3 = ArithmeticVector<int, 3>;
using int4 = ArithmeticVector<int, 4>;

using uint2 = ArithmeticVector<unsigned, 2>;
using uint3 = ArithmeticVector<unsigned, 3>;
using uint4 = ArithmeticVector<unsigned, 4>;

using float2 = ArithmeticVector<float, 2>;
using float3 = ArithmeticVector<float, 3>;
using float4 = ArithmeticVector<float, 4>;

namespace std
{
	template<> struct hash<uint3>
	{
		std::size_t operator()(const uint3& i) const noexcept
		{
			std::size_t h = std::hash<uint32_t>{}(i[0]);
			h ^= std::hash<uint32_t>{}(i[1]);
			h ^= std::hash<uint32_t>{}(i[2]);
			return h;
		}
	};
}

float3 cross(const float3& a, const float3& b);
float4 cross(const float4& a, const float4& b);

class Matrix3x3
{
public:
	// Defaults to identity matrix
	Matrix3x3();
	Matrix3x3(float3 row0, float3 row1, float3 row2);

	float& operator()(int row, int column);
	const float& operator()(int row, int column) const;

	float3 operator*(float3 vec);

    float3 row(int row) const;
    Matrix3x3 transpose() const;

    std::string debugOutput() const;

	friend Matrix3x3 operator*(const Matrix3x3& lhs, const Matrix3x3& rhs);
private:
	std::array<float, 9>	m_elements;	// Row-major order
};

class Matrix4x4
{
public:
	// Defaults to identity matrix
	Matrix4x4();
	Matrix4x4(float4 row0, float4 row1, float4 row2, float4 row3);

	float& operator()(int row, int column);
	const float& operator()(int row, int column) const;

	float4 operator*(float4 vec);

    float4 row(int row) const;
    Matrix4x4 transpose() const;

    std::string debugOutput() const;

	friend Matrix4x4 operator*(const Matrix4x4& lhs, const Matrix4x4& rhs);
private:
	std::array<float, 16>	m_elements;	// Row-major order
};

class Quaternion
{
public:
	Quaternion();
	Quaternion(float4 rotation);
	Quaternion(float4 axis, float angle);
    Quaternion(const Matrix3x3& rotation);

	bool operator==(const Quaternion& q) const;

	Quaternion  conjugate() const;
	float4      rotate(float4 position) const;
	
	Matrix4x4   toMatrix() const;
    float4      toFloat4() const;

	std::string debugOutput() const;

	friend Quaternion operator+(Quaternion a, const Quaternion& b);
	friend Quaternion operator-(Quaternion a, const Quaternion& b);
	friend Quaternion operator*(Quaternion a, const Quaternion& b);
private:
	float lengthSq();

	std::array<float, 4> m_rotation;
};

template<typename T, int N>
class Rect
{
public:
	Rect() : m_minCorner(ArithmeticVector<T, N>(0)), m_size(ArithmeticVector<T, N>(0)) {}
	Rect(ArithmeticVector<T, N> size) : m_minCorner(ArithmeticVector<T, N>(0)), m_size(size) {}
	Rect(ArithmeticVector<T, N> minCorner, ArithmeticVector<T, N> size) : m_minCorner(minCorner), m_size(size) {}

	ArithmeticVector<T, N> minCorner() const	{ return m_minCorner; }
	ArithmeticVector<T, N> size() const			{ return m_size; }
private:
	ArithmeticVector<T, N> m_minCorner;
	ArithmeticVector<T, N> m_size;
};

// Colors: Probably need a special class in the future
using Color3 = ArithmeticVector<float, 3>;
using Color4 = ArithmeticVector<float, 4>;

// Non-owning data range reference
template<typename T>
class Range
{
public:
    Range() :
        m_begin(nullptr),
        m_byteSize(0)
    {}

	Range(T *begin, size_t byteSize) :
		m_begin(begin),
		m_byteSize(byteSize)
	{}

	Range(std::vector<T>& vector) :
		m_begin(vector.data()),
		m_byteSize(vector.size() * sizeof(T))
	{}

	T*		begin()		{ return m_begin; }
	T*		end()		{ return m_begin + size(); }

	size_t	size() const		{ return m_byteSize / sizeof(T); }
	size_t  byteSize() const	{ return m_byteSize; }

	const T& operator[](uint32_t i) const { return m_begin[i]; }
    T& operator[](uint32_t i) { return m_begin[i]; }

	// TODO: Fill other member as required
private:
	T*		m_begin;
	size_t	m_byteSize;
};

template<typename T>
Range<const uint8_t> vectorAsByteRange(std::vector<T>& vector)
{
    return Range<const uint8_t>(reinterpret_cast<const uint8_t*>(vector.data()), vector.size() * sizeof(T));
}

// Owning data container
template<typename T = char>
class DataBlob
{
public:
    DataBlob() :
        m_data(nullptr)
    {}

    DataBlob(size_t elements)
    {
        m_data = std::make_shared<std::vector<T>>(elements);
    }

    void resize(size_t elements)
    {
        if (m_data == nullptr)
        {
            m_data = std::make_shared<std::vector<T>>(elements);
        }
        else
        {
            if (elements != m_data->size())
            {
                m_data->resize(elements);
            }
        }
    }

    size_t size() const      { return (m_data == nullptr) ? 0 :m_data->size(); }
    size_t bytesSize() const { return (m_data == nullptr) ? 0 : m_data->size() * sizeof(T); }

    T* data()                { return (m_data == nullptr) ? nullptr : m_data->data(); }
    const T* data() const    { return (m_data == nullptr) ? nullptr : m_data->data(); }
private:
    std::shared_ptr<std::vector<T>> m_data;
};

template<typename T>
Range<const uint8_t> blobAsByteRange(DataBlob<T>& vector)
{
    return Range<const uint8_t>(reinterpret_cast<const uint8_t*>(vector.data()), vector.size() * sizeof(T));
}
