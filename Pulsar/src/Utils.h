#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <functional>
#include <sstream>

class bad_permutation_error : public std::exception
{
};

class Permutation
{
	size_t n;
	std::vector<size_t> nums_vector;

public:
	Permutation() : n(0) {}
	Permutation(size_t n);
	Permutation(const std::vector<size_t>& nums);
	size_t operator[](const size_t& i) const;
	inline size_t size() const { return n; }
	Permutation inverse() const;
};

// TODO instead of Utils.h file, create utils folder with different utility files. so one for these function definitions, another for the concatenation functions, etc.

extern std::function<float(float)> ConstantFunc(float c);
extern std::function<float(float)> StepFunc(float at, float height = 1.0f, bool dirLR = true, bool incl = true, float lower = 0.0f);
extern std::function<float(float)> LinearFunc(float slope = 0, float intercept = 0);
extern std::function<float(float)> QuadraticFunc(float a, float x_intercept, float y_intercept);
extern std::function<float(float)> PowerFunc(float a, float exp);
extern std::function<float(float)> FuncSum(const std::function<float(float)>& f1, const std::function<float(float)>& f2);
extern std::function<float(float)> FuncMul(const std::function<float(float)>& f1, const std::function<float(float)>& f2);

extern std::function<glm::vec4(float)> LinearCombo4x1(const glm::vec4& constant, const glm::vec4& coeff1);
extern std::function<glm::vec4(const glm::vec2&)> LinearCombo4x2(const glm::vec4& constant, const glm::vec4& coeff1, const glm::vec4& coeff2);
extern std::function<glm::vec4(const glm::vec3&)> LinearCombo4x3(const glm::vec4& constant, const glm::vec4& coeff1, const glm::vec4& coeff2, const glm::vec4& coeff3);
extern std::function<glm::vec4(const glm::vec4&)> LinearCombo4x4(const glm::vec4& constant, const glm::vec4& coeff1, const glm::vec4& coeff2, const glm::vec4& coeff3, const glm::vec4& coeff4);
extern std::function<glm::vec3(float)> LinearCombo3x1(const glm::vec3& constant, const glm::vec3& coeff1);
extern std::function<glm::vec3(const glm::vec2&)> LinearCombo3x2(const glm::vec3& constant, const glm::vec3& coeff1, const glm::vec3& coeff2);
extern std::function<glm::vec3(const glm::vec3&)> LinearCombo3x3(const glm::vec3& constant, const glm::vec3& coeff1, const glm::vec3& coeff2, const glm::vec3& coeff3);
extern std::function<glm::vec3(const glm::vec4&)> LinearCombo3x4(const glm::vec3& constant, const glm::vec3& coeff1, const glm::vec3& coeff2, const glm::vec3& coeff3, const glm::vec3& coeff4);
extern std::function<glm::vec2(float)> LinearCombo2x1(const glm::vec2& constant, const glm::vec2& coeff1);
extern std::function<glm::vec2(const glm::vec2&)> LinearCombo2x2(const glm::vec2& constant, const glm::vec2& coeff1, const glm::vec2& coeff2);
extern std::function<glm::vec2(const glm::vec3&)> LinearCombo2x3(const glm::vec2& constant, const glm::vec2& coeff1, const glm::vec2& coeff2, const glm::vec2& coeff3);
extern std::function<glm::vec2(const glm::vec4&)> LinearCombo2x4(const glm::vec2& constant, const glm::vec2& coeff1, const glm::vec2& coeff2, const glm::vec2& coeff3, const glm::vec2& coeff4);

extern std::function<void(float&)> OperateFloatMult(float mult);
extern std::function<std::function<void(float&)>(float)> OperateFloatMultWrap;

extern std::function<unsigned int(float)> CastFloatToUInt;
extern std::function<float(float)> Identity;

template<typename T, typename U, typename V, typename W>
inline std::function<T(W)> Composition(const std::function<T(U)>& f, const std::function<V(W)>& g)
{
	return [f, g](W w) -> T { return f(static_cast<U>(g(w))); };
}

inline std::function<glm::vec2(const glm::vec2&)> Vec2Wrap(const std::function<float(float)>& f, const std::function<float(float)>& g)
{
	return [f, g](const glm::vec2& v) -> glm::vec2 { return { f(v[0]), f(v[1]) }; };
}

template<std::floating_point Float = float>
inline Float rng() { return std::rand() / static_cast<Float>(RAND_MAX); }

inline std::string Concat(const std::string& delim)
{
	return "";
}

template<typename T>
inline std::string Concat(const std::string& delta, const T& last)
{
	std::stringstream ss;
	ss << last;
	return ss.str();
}

template<typename T, typename... Args>
inline std::string Concat(const std::string& delim, const T& first, const Args&... args)
{
	std::stringstream ss;
	ss << first << delim;
	return ss.str() + Concat(delim, args...);
}

inline std::string STR(const glm::vec2& vec2)
{
	return Concat("", "<", vec2[0], ", ", vec2[1], ">");
}
inline std::string STR(const glm::vec3& vec3)
{
	return Concat("", "<", vec3[0], ", ", vec3[1], ", ", vec3[2], ">");
}
inline std::string STR(const glm::vec4& vec4)
{
	return Concat("", "<", vec4[0], ", ", vec4[1], ", ", vec4[2], ", ", vec4[3], ">");
}

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
	std::function<float(float)> cfunc;
	Int prev = 0;

	CumulativeFunc(const std::function<float(float)>& cf, Int initial = 0) : cfunc(cf), prev(initial) {}
	CumulativeFunc(const CumulativeFunc& func) : cfunc(func.cfunc), prev(func.prev) {}
	CumulativeFunc(CumulativeFunc&& func) noexcept : cfunc(std::move(func.cfunc)), prev(func.prev) {}
	CumulativeFunc& operator=(const CumulativeFunc& func) { cfunc = func.cfunc; prev = func.prev; return *this; }
	CumulativeFunc& operator=(CumulativeFunc&& func) noexcept { cfunc = std::move(func.cfunc); prev = func.prev; return *this; }

	inline Int operator()(float t)
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

template<typename T>
inline T Max(const T& first)
{
	return first;
}

template<typename T, typename... Args>
inline T Max(const T& first, const Args&... args)
{
	return std::max(first, static_cast<T>(Max(args...)));
}
