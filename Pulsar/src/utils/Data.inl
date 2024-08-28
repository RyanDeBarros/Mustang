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

template<typename T>
inline void insert_sorted(std::vector<T>& vec, T&& el)
{
	auto iter = std::lower_bound(vec.begin(), vec.end(), el);
	vec.insert(iter, std::forward<T>(el));
}

template<typename T, typename Compare>
inline void insert_sorted(std::vector<T>& vec, T&& el, Compare compare)
{
	auto iter = std::lower_bound(vec.begin(), vec.end(), el, compare);
	vec.insert(iter, std::forward<T>(el));
}

template<typename T, typename Equals>
inline void set_or_insert_sorted(std::vector<T>& vec, T&& el, Equals equals)
{
	auto iter = std::lower_bound(vec.begin(), vec.end(), el);
	if (iter != vec.end() && equals(*iter, el))
		*iter = std::forward<T>(el);
	else
		vec.insert(iter, std::forward<T>(el));
}

template<typename T, typename Equals, typename Compare>
inline void set_or_insert_sorted(std::vector<T>& vec, T&& el, Equals equals, Compare compare)
{
	auto iter = std::lower_bound(vec.begin(), vec.end(), el, compare);
	if (iter != vec.end() && equals(*iter, el))
		*iter = std::forward<T>(el);
	else
		vec.insert(iter, std::forward<T>(el));
}
