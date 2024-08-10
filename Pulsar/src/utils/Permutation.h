#pragma once

#include <vector>

class bad_permutation_error : public std::exception
{
};

class Permutation
{
	size_t m_N;
	std::vector<size_t> m_Nums;

public:
	Permutation() : m_N(0) {}
	Permutation(size_t n);
	Permutation(const std::vector<size_t>& nums);
	size_t operator[](size_t i) const;
	inline size_t size() const { return m_N; }
	Permutation inverse() const;
};

class SwapPopIndexer
{
	std::vector<size_t> m_Indices;

public:
	SwapPopIndexer(size_t initial_size);
	inline size_t operator[](size_t i) const { return m_Indices[i]; }
	
	void SwapPop(size_t i);
	void Push();
};
