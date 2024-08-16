#pragma once

#include <vector>

template<typename T>
inline void swap_pop(std::vector<T>& vec, size_t i)
{
	if (vec.size() > 1)
		std::swap(vec[i], vec.back());
	vec.pop_back();
}

template<typename T>
inline void set_ptr(T* ptr, const T& value)
{
	if (ptr)
		*ptr = value;
}

template<typename T>
inline void set_ptr(T* ptr, T&& value)
{
	if (ptr)
		*ptr = std::move(value);
}
