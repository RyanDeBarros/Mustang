#include "Permutation.h"

#include <unordered_set>
#include <string>
#include <stdexcept>

Permutation::Permutation(size_t n)
	: m_N(n)
{
	m_Nums.reserve(n);
	for (size_t i = 0; i < n; i++)
		m_Nums.push_back(i);
}

Permutation::Permutation(const std::vector<size_t>& nums)
	: m_N(0)
{
	std::unordered_set<size_t> nums_set;
	size_t maxi = 0;
	for (size_t i = 0; i < nums.size(); i++)
	{
		if (nums_set.find(nums[i]) == nums_set.end())
		{
			nums_set.insert(nums[i]);
			m_Nums.push_back(nums[i]);
			if (nums[i] > maxi)
				maxi = nums[i];
		}
	}
	if (maxi + 1 != m_Nums.size())
		throw bad_permutation_error();
	else
		m_N = m_Nums.size();
}

size_t Permutation::operator[](size_t i) const
{
	if (i >= m_N)
		throw std::out_of_range("Index " + std::to_string(i) + " is not less than permutation size " + std::to_string(m_N));
	return m_Nums[i];
}

Permutation Permutation::inverse() const
{
	std::vector<size_t> inverse;
	for (size_t i = 0; i < m_N; i++)
		inverse.push_back(0);
	for (size_t i = 0; i < m_N; i++)
		inverse[m_Nums[i]] = i;
	return inverse;
}

SwapPopIndexer::SwapPopIndexer(size_t initial_size)
{
	m_Indices.reserve(initial_size);
	for (size_t i = 0; i < initial_size; i++)
		m_Indices.push_back(i);
}

void SwapPopIndexer::SwapPop(size_t i)
{
	if (i >= m_Indices.size())
		throw bad_permutation_error();
	auto index = m_Indices[i];
	size_t n = 0;
	while (n < i)
	{
		if (m_Indices[n] == m_Indices.size() - 1)
			m_Indices[n] = index;
		n++;
	}
	while (n < m_Indices.size() - 1)
	{
		m_Indices[n] = m_Indices[n + 1];
		if (m_Indices[n] == m_Indices.size() - 1)
			m_Indices[n] = index;
		n++;
	}
	m_Indices.erase(m_Indices.begin() + n);
}

void SwapPopIndexer::Push()
{
	m_Indices.push_back(m_Indices.size());
}
