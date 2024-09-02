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

template<size_t Count>
struct _unfurl_loop_impl
{
	constexpr static void _(auto&& body)
	{
		_unfurl_loop_impl<Count - 1>::_(body);
		body(Count - 1);
	}
};

template<>
struct _unfurl_loop_impl<0>
{
	constexpr static void _(auto&& body)
	{
	}
};

template<size_t Count>
constexpr void unfurl_loop(auto&& body)
{
	_unfurl_loop_impl<Count>::_(body);
}

template<size_t Count>
struct _unfurl_loop_noindex_impl
{
	constexpr static void _(auto&& body)
	{
		_unfurl_loop_noindex_impl<Count - 1>::_(body);
		body();
	}
};

template<>
struct _unfurl_loop_noindex_impl<0>
{
	constexpr static void _(auto&& body)
	{
	}
};

template<size_t Count>
constexpr void unfurl_loop_noindex(auto&& body)
{
	_unfurl_loop_noindex_impl<Count>::_(body);
}
