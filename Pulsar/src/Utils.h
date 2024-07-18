#pragma once

#include <vector>
#include <functional>

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

extern std::function<float(float)> LinearFunc(float slope = 0, float intercept = 0);

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
	std::function<Int(float)> cfunc;
	Int prev = 0;

	CumulativeFunc(const std::function<Int(float)>& cf, Int initial = 0) : cfunc(cf), prev(initial) {}
	CumulativeFunc(const CumulativeFunc& func) : cfunc(func.cfunc), prev(func.prev) {}

	inline Int operator()(float t)
	{
		Int res = static_cast<Int>(std::lroundf(t));
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
