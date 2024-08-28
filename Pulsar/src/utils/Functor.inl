#pragma once

#include <type_traits>

#ifndef PULSAR_FORWARD
#define PULSAR_FORWARD(var) std::forward<decltype(var)>(var)
#endif

template<typename T>
decltype(auto) redirect(T arg)
{
	if constexpr (std::is_lvalue_reference_v<T>)
		return (arg);
	else if constexpr (std::is_rvalue_reference_v<T>)
		return std::move(arg);
	else
		return arg;
}

template<typename Ret, typename Arg>
struct FunctorInterface
{
	// TODO for virtual inheritance, add more virtual destructors.
	inline virtual ~FunctorInterface() = default;
	inline virtual Ret operator()(Arg arg) = 0;
	inline virtual FunctorInterface<Ret, Arg>* Clone() = 0;
};

template<typename Ret>
struct FunctorInterface<Ret, void>
{
	inline virtual ~FunctorInterface() = default;
	inline virtual Ret operator()() = 0;
	inline virtual FunctorInterface<Ret, void>* Clone() = 0;
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

template<typename T>
struct strip_rvalue_reference
{
	using type = T;
};

template<typename T>
struct strip_rvalue_reference<T&&>
{
	using type = T;
};

template<typename T>
using strip_rvalue_reference_t = strip_rvalue_reference<T>::type;

template<typename Ret, typename Arg, typename Cls, typename _Storage, bool _ValueIn = true>
struct Functor : public FunctorInterface<Ret, Arg>
{
	static_assert(!std::is_lvalue_reference_v<_Storage>, "Functor: _Storage cannot be l-value reference when _ValueIn=true.");
	static_assert(!std::is_rvalue_reference_v<_Storage>, "Functor: _Storage cannot be r-value reference.");
	static_assert(std::is_convertible_v<_Storage, Cls>, "Functor: _Storage is not convertible to Cls.");
	using func_signature = func_signature_t<Ret, Arg, Cls>;

	inline Functor(func_signature function, _Storage closure) : function(function), closure(closure) {}

	inline Ret operator()(Arg arg) override
	{
		if constexpr (std::is_void_v<Ret>)
			function(redirect<Arg>(arg), redirect<Cls>(closure));
		else
			return function(redirect<Arg>(arg), redirect<Cls>(closure));
	}
	inline FunctorInterface<Ret, Arg>* Clone() override
	{
		return new Functor<Ret, Arg, Cls, _Storage, true>(function, closure);
	}

	func_signature function;
	_Storage closure;
};

template<typename Ret, typename Arg, typename Cls, typename _Storage>
struct Functor<Ret, Arg, Cls, _Storage, false> : public FunctorInterface<Ret, Arg>
{
	static_assert(!std::is_rvalue_reference_v<_Storage>, "Functor: _Storage cannot be r-value reference.");
	static_assert(std::is_convertible_v<_Storage, Cls>, "Functor: _Storage is not convertible to Cls.");
	using func_signature = func_signature_t<Ret, Arg, Cls>;

	inline Functor(func_signature function, _Storage&& closure) : function(function), closure(PULSAR_FORWARD(closure)) {}

	inline Ret operator()(Arg arg) override
	{
		if constexpr (std::is_void_v<Ret>)
			function(redirect<Arg>(arg), redirect<Cls>(closure));
		else
			return function(redirect<Arg>(arg), redirect<Cls>(closure));
	}
	inline FunctorInterface<Ret, Arg>* Clone() override
	{
		return new Functor<Ret, Arg, Cls, _Storage, false>(function, PULSAR_FORWARD(closure));
	}

	func_signature function;
	_Storage closure;
};

template<typename Ret, typename Arg, typename _Storage, bool _ValueIn>
struct Functor<Ret, Arg, void, _Storage, _ValueIn> : public FunctorInterface<Ret, Arg>
{
	static_assert(std::is_void_v<_Storage>, "Functor: _Storage must be void when Cls is void.");
	using func_signature = func_signature_t<Ret, Arg, void>;

	inline Functor(func_signature function) : function(function) {}

	inline Ret operator()(Arg arg) override
	{
		if constexpr (std::is_void_v<Ret>)
			function(redirect<Arg>(arg));
		else
			return function(redirect<Arg>(arg));
	}
	inline FunctorInterface<Ret, Arg>* Clone() override
	{
		return new Functor<Ret, Arg, void, void, _ValueIn>(function);
	}

	func_signature function;
};

template<typename Ret, typename Cls, typename _Storage>
struct Functor<Ret, void, Cls, _Storage, true> : public FunctorInterface<Ret, void>
{
	static_assert(!std::is_lvalue_reference_v<_Storage>, "Functor: _Storage cannot be l-value reference when _ValueIn=true.");
	static_assert(!std::is_rvalue_reference_v<_Storage>, "Functor: _Storage cannot be r-value reference.");
	static_assert(std::is_convertible_v<_Storage, Cls>, "Functor: _Storage is not convertible to Cls.");
	using func_signature = func_signature_t<Ret, void, Cls>;

	inline Functor(func_signature function, _Storage closure) : function(function), closure(closure) {}

	inline Ret operator()() override
	{
		if constexpr (std::is_void_v<Ret>)
			function(redirect<Cls>(closure));
		else
			return function(redirect<Cls>(closure));
	}
	inline FunctorInterface<Ret, void>* Clone() override
	{
		return new Functor<Ret, void, Cls, _Storage, true>(function, closure);
	}

	func_signature function;
	_Storage closure;
};

template<typename Ret, typename Cls, typename _Storage>
struct Functor<Ret, void, Cls, _Storage, false> : public FunctorInterface<Ret, void>
{
	static_assert(!std::is_rvalue_reference_v<_Storage>, "Functor: _Storage cannot be r-value reference.");
	static_assert(std::is_convertible_v<_Storage, Cls>, "Functor: _Storage is not convertible to Cls.");
	using func_signature = func_signature_t<Ret, void, Cls>;

	inline Functor(func_signature function, _Storage&& closure) : function(function), closure(PULSAR_FORWARD(closure)) {}

	inline Ret operator()() override
	{
		if constexpr (std::is_void_v<Ret>)
			function(redirect<Cls>(closure));
		else
			return function(redirect<Cls>(closure));
	}
	inline FunctorInterface<Ret, void>* Clone() override
	{
		return new Functor<Ret, void, Cls, _Storage, false>(function, PULSAR_FORWARD(closure));
	}

	func_signature function;
	_Storage closure;
};

template<typename Ret, typename _Storage, bool _ValueIn>
struct Functor<Ret, void, void, _Storage, _ValueIn> : public FunctorInterface<Ret, void>
{
	static_assert(std::is_void_v<_Storage>, "Functor: _Storage must be void when Cls is void.");
	using func_signature = func_signature_t<Ret, void, void>;

	inline Functor(func_signature function) : function(function) {}

	inline Ret operator()() override
	{
		if constexpr (std::is_void_v<Ret>)
			function();
		else
			return function();
	}
	inline FunctorInterface<Ret, void>* Clone() override
	{
		return new Functor<Ret, void, void, void, _ValueIn>(function);
	}

	func_signature function;
};

template<typename Ret, typename Arg>
class FunctorPtr
{
	FunctorInterface<Ret, Arg>* f = nullptr;

public:
	using RetType = Ret;
	using ArgType = Arg;

	template<typename Cls, typename _Storage, bool _ValueIn>
	inline FunctorPtr(Functor<Ret, Arg, Cls, _Storage, _ValueIn>* f) : f(f) {}
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

	// TODO dangerous to dereference. Define macro that can check if f is null first. OR use func pointer assignment in constructor or whenever f could be set null. In which case, operator()() should throw.
	// Also, even if f is non-null, f.function may be null.
	inline Ret operator()(Arg arg) const
	{
		if constexpr (std::is_void_v<Ret>)
			(*f)(arg);
		else
			return (*f)(arg);
	}
};

template<typename Ret>
class FunctorPtr<Ret, void>
{
	FunctorInterface<Ret, void>* f = nullptr;

public:
	using RetType = Ret;
	using ArgType = void;

	template<typename Cls, typename _Storage, bool _ValueIn>
	inline FunctorPtr(Functor<Ret, void, Cls, _Storage, _ValueIn>* f) : f(f) {}
	inline FunctorPtr(FunctorInterface<Ret, void>* f) : f(f) {}
	inline FunctorPtr(const FunctorPtr<Ret, void>& other)
	{
		if (other.f)
			f = other.f->Clone();
	}
	inline FunctorPtr(FunctorPtr<Ret, void>&& other) noexcept : f(other.f) { other.f = nullptr; }
	inline FunctorPtr& operator=(const FunctorPtr<Ret, void>& other)
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
	inline FunctorPtr<Ret, void> Clone() const
	{
		return f ? FunctorPtr<Ret, void>(f->Clone()) : nullptr;
	}

	// TODO dangerous to dereference. Define macro that can check if f is null first.
	inline Ret operator()() const
	{
		if constexpr (std::is_void_v<Ret>)
			(*f)();
		else
			return (*f)();
	}
};

template<typename T>
struct is_functor_ptr : public std::false_type {};

template<typename Ret, typename Arg>
struct is_functor_ptr<FunctorPtr<Ret, Arg>> : public std::true_type {};

template<typename T>
inline constexpr bool is_functor_ptr_v = is_functor_ptr<T>::value;

template<typename T>
inline constexpr bool decays_to_functor_ptr_v = is_functor_ptr_v<std::decay_t<T>>;

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

template<bool _ValueIn, bool _ReferExternal = true>
inline auto make_functor_ptr(auto f, auto&& closure) requires (!_ValueIn)
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
		if constexpr (_ReferExternal)
		{
			using _Storage = strip_rvalue_reference_t<decltype(closure)>;
			return FunctorPtr<Ret, Arg>(new Functor<Ret, Arg, Cls, _Storage, false>(f, PULSAR_FORWARD(closure)));
		}
		else
		{
			using _Storage = std::remove_reference_t<decltype(closure)>;
			return FunctorPtr<Ret, Arg>(new Functor<Ret, Arg, Cls, _Storage, false>(f, PULSAR_FORWARD(closure)));
		}
	}
	else
	{
		using Ret = parse_function<Func>::ret_type;
		using Cls = parse_function<Func>::par_type;
		static_assert(std::is_same_v<std::decay_t<Cls>, std::decay_t<decltype(closure)>>);
		static_assert(std::is_same_v<Func, func_signature_t<Ret, void, Cls>>);
		if constexpr (_ReferExternal)
		{
			using _Storage = strip_rvalue_reference_t<decltype(closure)>;
			return FunctorPtr<Ret, void>(new Functor<Ret, void, Cls, _Storage, false>(f, PULSAR_FORWARD(closure)));
		}
		else
		{
			using _Storage = std::remove_reference_t<decltype(closure)>;
			return FunctorPtr<Ret, void>(new Functor<Ret, void, Cls, _Storage, false>(f, PULSAR_FORWARD(closure)));
		}
	}
}

template<bool _ValueIn>
inline auto make_functor_ptr(auto f, auto closure) requires (_ValueIn)
{
	using Func = decltype(+f);
	constexpr auto parse_v = parse_function_v<Func>;
	static_assert(parse_v == 1 || parse_v == 2);
	if constexpr (parse_v == 1)
	{
		using Ret = parse_function<Func>::ret_type;
		using Arg = parse_function<Func>::arg_type;
		using Cls = parse_function<Func>::cls_type;
		using _Storage = decltype(closure);
		static_assert(std::is_same_v<std::decay_t<Cls>, std::decay_t<decltype(closure)>>);
		static_assert(std::is_same_v<Func, func_signature_t<Ret, Arg, Cls>>);
		return FunctorPtr<Ret, Arg>(new Functor<Ret, Arg, Cls, _Storage, true>(f, closure));
	}
	else
	{
		using Ret = parse_function<Func>::ret_type;
		using Cls = parse_function<Func>::par_type;
		using _Storage = decltype(closure);
		static_assert(std::is_same_v<std::decay_t<Cls>, std::decay_t<decltype(closure)>>);
		static_assert(std::is_same_v<Func, func_signature_t<Ret, void, Cls>>);
		return FunctorPtr<Ret, void>(new Functor<Ret, void, Cls, _Storage, true>(f, closure));
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
		return FunctorPtr<Ret, Arg>(new Functor<Ret, Arg, void, void, true>(f));
	}
	else
	{
		static_assert(std::is_same_v<Func, func_signature_t<Ret, void, void>>);
		return FunctorPtr<Ret, void>(new Functor<Ret, void, void, void, true>(f));
	}
}

inline FunctorPtr<void, void> VoidFunctorPtr = make_functor_ptr([]() {});
