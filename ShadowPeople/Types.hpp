#pragma once

#include <array>

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

	/*
	ArithmeticVector(const ArithmeticVector&) = default;
	ArithmeticVector(const ArithmeticVector&&) = default;

	ArithmeticVector& operator=(const ArithmeticVector&) = default;
	ArithmeticVector& operator=(const ArithmeticVector&&) = default;
	*/
	bool operator==(const ArithmeticVector& rhs)
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

template<typename T, int N>
class Rect
{
public:
	Rect() : m_minCorner(ArithmeticVector<T, N>(0)), m_size(ArithmeticVector<T, N>(0)) {}
	Rect(ArithmeticVector<T, N> size) : m_minCorner(ArithmeticVector<T, N>(0)), m_size(size) {}
	Rect(ArithmeticVector<T, N> minCorner, ArithmeticVector<T, N> size) : m_minCorner(minCorner), m_size(size) {}

	ArithmeticVector<T, N> minCorner()		{ return m_minCorner; }
	ArithmeticVector<T, N> size()			{ return m_size; }
private:
	ArithmeticVector<T, N> m_minCorner;
	ArithmeticVector<T, N> m_size;
};

// Colors: Probably need a special class in the future
using Color3 = ArithmeticVector<float, 3>;
using Color4 = ArithmeticVector<float, 4>;

template<typename T>
class Range
{
public:
	Range(T *begin, size_t byteSize) :
		m_begin(begin),
		m_byteSize(byteSize)
	{}

	T*		begin()		{ return m_begin; }
	size_t  byteSize()	{ return m_byteSize; }
	// TODO: Fill other member as required
private:
	T*		m_begin;
	size_t	m_byteSize;
};
