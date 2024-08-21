#pragma once

#include <type_traits>

#ifndef PULSAR_FORWARD
#define PULSAR_FORWARD(var) std::forward<decltype(var)>(var)
#endif

template<typename Ret, typename Arg>
struct FunctorInterface
{
	// TODO for virtual inheritance, add more virtual destructors.
	inline virtual ~FunctorInterface() = default;
	inline virtual Ret operator()(Arg arg) = 0;
	inline virtual FunctorInterface<Ret, Arg>* Clone() const = 0;
};

template<typename Ret>
struct FunctorInterface<Ret, void>
{
	inline virtual ~FunctorInterface() = default;
	inline virtual Ret operator()() = 0;
	inline virtual FunctorInterface<Ret, void>* Clone() const = 0;
};

template<typename Ret, typename Arg, typename Cls>
struct func_signature
{
	using type = Ret(*)(Arg, Cls);
};

template<typename Ret, typename Cls>
struct func_signature<Ret, void, Cls>
{
	using type = Ret(*)(Cls);
};

template<typename Ret, typename Arg>
struct func_signature<Ret, Arg, void>
{
	using type = Ret(*)(Arg);
};

template<typename Ret>
struct func_signature<Ret, void, void>
{
	using type = Ret(*)();
};

template<typename Ret, typename Arg, typename Cls>
using func_signature_t = func_signature<Ret, Arg, Cls>::type;

template<typename Ret, typename Arg, typename Cls, bool ClosureCopy = false>
struct Functor : public FunctorInterface<Ret, Arg>
{
	using func_signature = func_signature_t<Ret, Arg, Cls>;

	inline Functor(func_signature function, Cls&& closure) : function(function), closure(std::forward<Cls>(closure)) {}

	inline Ret operator()(Arg arg) override
	{
		if constexpr (std::is_void_v<Ret>)
			function(arg, closure);
		else
			return function(arg, closure);
	}
	inline FunctorInterface<Ret, Arg>* Clone() const override
	{
		return new Functor<Ret, Arg, Cls>(function, closure);
	}

	func_signature function;
	std::decay_t<Cls> closure;
};

template<typename Ret, typename Arg, typename Cls>
struct Functor<Ret, Arg, Cls, true> : public FunctorInterface<Ret, Arg>
{
	using func_signature = func_signature_t<Ret, Arg, Cls>;

	inline Functor(func_signature function, Cls closure) : function(function), closure(closure) {}

	inline Ret operator()(Arg arg) override
	{
		if constexpr (std::is_void_v<Ret>)
			function(arg, closure);
		else
			return function(arg, closure);
	}
	inline FunctorInterface<Ret, Arg>* Clone() const override
	{
		return new Functor<Ret, Arg, Cls, true>(function, closure);
	}

	func_signature function;
	std::decay_t<Cls> closure;
};

template<typename Ret, typename Arg, bool CopyClosure>
struct Functor<Ret, Arg, void, CopyClosure> : public FunctorInterface<Ret, Arg>
{
	using func_signature = func_signature_t<Ret, Arg, void>;

	inline Functor(func_signature function) : function(function) {}

	inline Ret operator()(Arg arg) override
	{
		if constexpr (std::is_void_v<Ret>)
			function(arg);
		else
			return function(arg);
	}
	inline FunctorInterface<Ret, Arg>* Clone() const override
	{
		return new Functor<Ret, Arg, void>(function);
	}

	func_signature function;
};

template<typename Ret, typename Cls>
struct Functor<Ret, void, Cls, false> : public FunctorInterface<Ret, void>
{
	using func_signature = func_signature_t<Ret, void, Cls>;

	inline Functor(func_signature function, Cls&& closure) : function(function), closure(std::forward<Cls>(closure)) {}

	inline Ret operator()() override
	{
		if constexpr (std::is_void_v<Ret>)
			function(closure);
		else
			return function(closure);
	}
	inline FunctorInterface<Ret, void>* Clone() const override
	{
		return new Functor<Ret, void, Cls>(function, closure);
	}

	func_signature function;
	std::decay_t<Cls> closure;
};

template<typename Ret, typename Cls>
struct Functor<Ret, void, Cls, true> : public FunctorInterface<Ret, void>
{
	using func_signature = func_signature_t<Ret, void, Cls>;

	inline Functor(func_signature function, Cls closure) : function(function), closure(closure) {}

	inline Ret operator()() override
	{
		if constexpr (std::is_void_v<Ret>)
			function(closure);
		else
			return function(closure);
	}
	inline FunctorInterface<Ret, void>* Clone() const override
	{
		return new Functor<Ret, void, Cls, true>(function, closure);
	}

	func_signature function;
	std::decay_t<Cls> closure;
};

template<typename Ret, bool CopyClosure>
struct Functor<Ret, void, void, CopyClosure> : public FunctorInterface<Ret, void>
{
	using func_signature = func_signature_t<Ret, void, void>;

	inline Functor(func_signature function) : function(function) {}

	inline Ret operator()() override
	{
		if constexpr (std::is_void_v<Ret>)
			function();
		else
			return function();
	}
	inline FunctorInterface<Ret, void>* Clone() const override
	{
		return new Functor<Ret, void, void>(function);
	}

	func_signature function;
};

template<typename T1, typename T2>
struct check_void
{
	static constexpr unsigned char value =
		std::is_void_v<T1> ? (std::is_void_v<T2> ? 3 : 2)
		: (std::is_void_v<T2> ? 1 : 0);
};

template<typename T1, typename T2>
inline constexpr unsigned char check_void_v = check_void<T1, T2>::value;

template<typename Ret, typename Arg>
class FunctorPtr
{
	FunctorInterface<Ret, Arg>* f = nullptr;

public:
	using RetType = Ret;
	using ArgType = Arg;

	template<typename Cls>
	inline FunctorPtr(Functor<Ret, Arg, Cls>* f) : f(f) {}
	inline FunctorPtr(FunctorInterface<Ret, Arg>* f) : f(f) {}
	inline FunctorPtr(const FunctorPtr<Ret, Arg>& other)
	{
		if (other.f)
			f = other.f->Clone();
	}
	inline FunctorPtr(FunctorPtr<Ret, Arg>&& other) noexcept : f(other.f) { other.f = nullptr; }
	inline FunctorPtr& operator=(const FunctorPtr<Ret, Arg>& other)
	{
		if (f)
			delete f;
		if (other.f)
			f = other.f->Clone();
		else
			f = nullptr;
		return *this;
	}
	inline FunctorPtr& operator=(FunctorPtr&& other) noexcept
	{
		if (this == &other)
			return *this;
		if (f)
			delete f;
		f = other.f;
		other.f = nullptr;
		return *this;
	}
	inline ~FunctorPtr() { if (f) delete f; }
	inline FunctorPtr<Ret, Arg> Clone() const
	{
		return f ? FunctorPtr<Ret, Arg>(f->Clone()) : nullptr;
	}

	// TODO dangerous to dereference. Define macro that can check if f is null first.
	inline Ret operator()(Arg arg) const	requires (check_void_v<Ret, Arg> == 0) { return (*f)(arg); }
	inline Ret operator()()	const			requires (check_void_v<Ret, Arg> == 1) { return (*f)(); }
	inline void operator()(Arg arg) const	requires (check_void_v<Ret, Arg> == 2) { (*f)(arg); }
	inline void operator()() const			requires (check_void_v<Ret, Arg> == 3) { (*f)(); }
};

template<typename T>
struct is_functor_ptr : public std::false_type {};

template<typename Ret, typename Arg>
struct is_functor_ptr<FunctorPtr<Ret, Arg>> : public std::true_type {};

template<typename T>
inline constexpr bool is_functor_ptr_v = is_functor_ptr<std::decay_t<T>>::value;

template<typename Lambda>
struct parse_function
{
	static constexpr unsigned char value = 0;
};

template<typename Ret, typename Arg, typename Cls>
struct parse_function<Ret(*)(Arg, Cls)>
{
	static constexpr unsigned char value = 1;
	using ret_type = Ret;
	using arg_type = Arg;
	using cls_type = Cls;
};

template<typename Ret, typename Par>
struct parse_function<Ret(*)(Par)>
{
	static constexpr unsigned char value = 2;
	using ret_type = Ret;
	using par_type = Par;
};

template<typename Ret>
struct parse_function<Ret(*)()>
{
	static constexpr unsigned char value = 3;
	using ret_type = Ret;
};

template<typename Lambda>
inline constexpr unsigned char parse_function_v = parse_function<Lambda>::value;

template<bool CopyClosure = false>
inline auto make_functor_ptr(auto f, auto&& closure) requires (!CopyClosure)
{
	using Func = decltype(+f);
	constexpr auto parse_v = parse_function_v<Func>;
	static_assert(parse_v == 1 || parse_v == 2);
	if constexpr (parse_v == 1)
	{
		using Ret = parse_function<Func>::ret_type;
		using Arg = parse_function<Func>::arg_type;
		using Cls = parse_function<Func>::cls_type;
		static_assert(std::is_same_v<std::decay_t<Cls>, std::decay_t<decltype(closure)>>);
		static_assert(std::is_same_v<Func, func_signature_t<Ret, Arg, Cls>>);
		return FunctorPtr<Ret, Arg>(new Functor<Ret, Arg, Cls>(f, PULSAR_FORWARD(closure)));
	}
	else
	{
		using Ret = parse_function<Func>::ret_type;
		using Cls = parse_function<Func>::par_type;
		static_assert(std::is_same_v<std::decay_t<Cls>, std::decay_t<decltype(closure)>>);
		static_assert(std::is_same_v<Func, func_signature_t<Ret, void, Cls>>);
		return FunctorPtr<Ret, void>(new Functor<Ret, void, Cls>(f, PULSAR_FORWARD(closure)));
	}
}

template<bool CopyClosure>
inline auto make_functor_ptr(auto f, auto closure) requires (CopyClosure)
{
	using Func = decltype(+f);
	constexpr auto parse_v = parse_function_v<Func>;
	static_assert(parse_v == 1 || parse_v == 2);
	if constexpr (parse_v == 1)
	{
		using Ret = parse_function<Func>::ret_type;
		using Arg = parse_function<Func>::arg_type;
		using Cls = parse_function<Func>::cls_type;
		static_assert(std::is_same_v<std::decay_t<Cls>, std::decay_t<decltype(closure)>>);
		static_assert(std::is_same_v<Func, func_signature_t<Ret, Arg, Cls>>);
		return FunctorPtr<Ret, Arg>(new Functor<Ret, Arg, Cls, true>(f, closure));
	}
	else
	{
		using Ret = parse_function<Func>::ret_type;
		using Cls = parse_function<Func>::par_type;
		static_assert(std::is_same_v<std::decay_t<Cls>, std::decay_t<decltype(closure)>>);
		static_assert(std::is_same_v<Func, func_signature_t<Ret, void, Cls>>);
		return FunctorPtr<Ret, void>(new Functor<Ret, void, Cls, true>(f, closure));
	}
}

inline auto make_functor_ptr(auto f)
{
	using Func = decltype(+f);
	constexpr auto parse_v = parse_function_v<Func>;
	static_assert(parse_v == 2 || parse_v == 3);
	using Ret = parse_function<Func>::ret_type;
	if constexpr (parse_v == 2)
	{
		using Arg = parse_function<Func>::par_type;
		static_assert(std::is_same_v<Func, func_signature_t<Ret, Arg, void>>);
		return FunctorPtr<Ret, Arg>(new Functor<Ret, Arg, void>(f));
	}
	else
	{
		static_assert(std::is_same_v<Func, func_signature_t<Ret, void, void>>);
		return FunctorPtr<Ret, void>(new Functor<Ret, void, void>(f));
	}
}
