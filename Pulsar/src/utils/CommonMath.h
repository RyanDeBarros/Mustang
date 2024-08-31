#pragma once

#include <glm/glm.hpp>
#include <functional>
#include <limits>

#include "Functor.inl"

template<std::floating_point Float = float>
inline Float rng() { return std::rand() / static_cast<Float>(RAND_MAX); }

/// cfunc is assumed to be cumulative on the interval [0, 1). For example, consider cfunc to be f(t) = 5t. Then consider updates with the t-values that increment by 0.1:
/// 
/// t	| f(t) | prev | num = round(f(t)) - prev
/// 0.0 | 0.0  | 0	  | 0
/// 0.1 | 0.5  | 0	  | 1 - 0 = 1
/// 0.2 | 1.0  | 1	  | 1 - 1 = 0
/// 0.3 | 1.5  | 1	  | 2 - 1 = 1
/// 0.4 | 2.0  | 2	  | 2 - 2 = 0
/// 0.5 | 2.5  | 2	  | 3 - 2 = 1
/// 0.6 | 3.0  | 3	  | 3 - 3 = 0
/// 0.7 | 3.5  | 3	  | 4 - 3 = 1
/// 0.8 | 4.0  | 4	  | 4 - 4 = 0
/// 0.9 | 4.5  | 4	  | 5 - 4 = 1
/// 
/// So if a "constant" rate is needed, use f(t) = nt, where n is that rate.
template<std::unsigned_integral Int = unsigned short>
struct CumulativeFunc
{
	FunctorPtr<float, float> cfunc;
	Int prev = 0;

	CumulativeFunc(FunctorPtr<float, float>&& cf, Int initial = 0) : cfunc(std::move(cf)), prev(initial) {}
	CumulativeFunc(const CumulativeFunc& func) : cfunc(func.cfunc.Clone()), prev(func.prev) {}
	CumulativeFunc(CumulativeFunc&& func) noexcept : cfunc(std::move(func.cfunc)), prev(func.prev) {}
	CumulativeFunc& operator=(const CumulativeFunc& func) { cfunc = func.cfunc.Clone(); prev = func.prev; return *this; }
	CumulativeFunc& operator=(CumulativeFunc&& func) noexcept { cfunc = std::move(func.cfunc); prev = func.prev; return *this; }

	Int operator()(float t)
	{
		Int res = static_cast<Int>(std::lroundf(cfunc(t)));
		if (res >= prev)
		{
			Int num = res - prev;
			prev += num;
			return num;
		}
		else
		{
			prev = res;
			return res;
		}
	}
};

// TODO use constexpr for compatible functions
template<typename T>
constexpr T Max(const T& first)
{
	return first;
}

template<typename T, typename... Args>
constexpr T Max(const T& first, const Args&... args)
{
	return std::max(first, static_cast<T>(Max(args...)));
}

inline auto unsigned_mod(auto x, auto y)
{
	return (x % y + y) % y;
}

inline auto unsigned_fmod(auto x, auto y)
{
	return std::fmod(std::fmod(x, y) + y, y);
}

template<typename T = float>
inline bool SafeToDivide(T denominator)
{
	return std::abs(denominator) > std::numeric_limits<T>::epsilon();
}

inline glm::vec2 DivideOrZero(float f, const glm::vec2& v)
{
	return { SafeToDivide<>(v.x) ? f / v.x : 0.0f, SafeToDivide<>(v.y) ? f / v.y : 0.0f };
}

inline glm::vec4 DivideOrZero(const glm::vec4& n, const glm::vec4& d)
{
	return {
		SafeToDivide<>(d.r) ? n.r / d.r : 0.0f,
		SafeToDivide<>(d.g) ? n.g / d.g : 0.0f,
		SafeToDivide<>(d.b) ? n.b / d.b : 0.0f,
		SafeToDivide<>(d.a) ? n.a / d.a : 0.0f
	};
}
