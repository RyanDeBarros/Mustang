#include "Utils.h"

#include <unordered_set>
#include <stdexcept>
#include <string>

Permutation::Permutation(size_t n)
	: n(n)
{
	for (size_t i = 0; i < n; i++)
		nums_vector.push_back(i);
}

Permutation::Permutation(const std::vector<size_t>& nums)
	: n(0)
{
	std::unordered_set<size_t> nums_set;
	size_t maxi = 0;
	for (size_t i = 0; i < nums.size(); i++)
	{
		if (nums_set.find(nums[i]) == nums_set.end())
		{
			nums_set.insert(nums[i]);
			nums_vector.push_back(nums[i]);
			if (nums[i] > maxi)
				maxi = nums[i];
		}
	}
	if (maxi + 1 != nums_vector.size())
		throw bad_permutation_error();
	else
		n = nums_vector.size();
}

size_t Permutation::operator[](const size_t& i) const
{
	if (i >= n)
		throw std::out_of_range("Index " + std::to_string(i) + " is not less than permutation size " + std::to_string(n));
	return nums_vector[i];
}

Permutation Permutation::inverse() const
{
	std::vector<size_t> inverse;
	for (size_t i = 0; i < n; i++)
		inverse.push_back(0);
	for (size_t i = 0; i < n; i++)
		inverse[nums_vector[i]] = i;
	return inverse;
}

std::function<float(float)> ConstantFunc(float c)
{
	return [=](float t) -> float { return c; };
}

std::function<float(float)> StepFunc(float at, float height, bool dirLR, bool incl, float lower)
{
	if (dirLR)
		return [=](float t) -> float { return (t < at) || (!incl && t == at) ? lower : height; };
	else
		return [=](float t) -> float { return (t > at) || (!incl && t == at) ? lower : height; };
}

std::function<float(float)> LinearFunc(float slope, float intercept)
{
	return [=](float t) -> float { return slope * t + intercept; };
}

std::function<float(float)> QuadraticFunc(float a, float x_intercept, float y_intercept)
{
	return [=](float t) -> float { return a * glm::pow(t - x_intercept, 2) + y_intercept; };
}

std::function<float(float)> PowerFunc(float a, float exp)
{
	return [=](float t) -> float { return a * glm::pow(t, exp); };
}

std::function<float(float)> FuncSum(const std::function<float(float)>& f1, const std::function<float(float)>& f2)
{
	return [&](float t) -> float { return f1(t) + f2(t); };
}

std::function<float(float)> FuncMul(const std::function<float(float)>& f1, const std::function<float(float)>& f2)
{
	return [&](float t) -> float { return f1(t) * f2(t); };
}
