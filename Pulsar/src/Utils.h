#pragma once

#include <vector>

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
