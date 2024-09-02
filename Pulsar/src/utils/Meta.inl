#pragma once

constexpr decltype(auto) forward(auto&& var)
{
	return std::forward<decltype(var)>(var);
}

template<typename T>
constexpr decltype(auto) redirect(T arg)
{
	if constexpr (std::is_lvalue_reference_v<T>)
		return (arg);
	else if constexpr (std::is_rvalue_reference_v<T>)
		return std::move(arg);
	else
		return arg;
}

template<typename T>
constexpr T& remove_const(const T& el)
{
	return *const_cast<T*>(&el);
}
