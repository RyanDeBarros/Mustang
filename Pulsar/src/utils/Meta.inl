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

template<size_t Until>
struct _unfurl_loop_impl
{
	constexpr static void _(auto&& body)
	{
		_unfurl_loop_impl<Until - 1>::_(body);
		body(Until);
	}
};

template<>
struct _unfurl_loop_impl<0>
{
	constexpr static void _(auto&& body)
	{
		body(0);
	}
};

template<size_t Count>
constexpr void unfurl_loop(auto&& body)
{
	if constexpr (Count > 0)
		_unfurl_loop_impl<Count - 1>::_(body);
}
