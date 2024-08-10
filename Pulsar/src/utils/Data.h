#pragma once

#include <vector>

template<typename T>
inline void swap_pop(std::vector<T>& vec, size_t i)
{
	if (vec.size() > 1)
		std::swap(vec[i], vec.back());
	vec.pop_back();
}
