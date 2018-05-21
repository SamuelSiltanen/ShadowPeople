#pragma once

namespace math
{
	template<typename T>
	T divRoundUp(T a, T b)
	{
		return (a % b) ? (a / b + 1) : (a / b);
	}
}
