#pragma once

#include <type_traits>

#include "utils/Meta.inl"

template<typename Ret, typename Arg>
struct _FunctorInterface
{
	virtual ~_FunctorInterface() = default;
	virtual Ret operator()(Arg arg) = 0;
	virtual _FunctorInterface<Ret, Arg>* clone() = 0;
};

template<typename Ret>
struct _FunctorInterface<Ret, void>
{
	virtual ~_FunctorInterface() = default;
	virtual Ret operator()() = 0;
	virtual _FunctorInterface<Ret, void>* clone() = 0;
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
struct _FunctorEnclosure : public _FunctorInterface<Ret, Arg>
{
	static_assert(!std::is_lvalue_reference_v<_Storage>, "_FunctorEnclosure: _Storage cannot be l-value reference when _ValueIn=true.");
	static_assert(!std::is_rvalue_reference_v<_Storage>, "_FunctorEnclosure: _Storage cannot be r-value reference.");
	static_assert(std::is_convertible_v<_Storage, Cls>, "_FunctorEnclosure: _Storage is not convertible to Cls.");
	static_assert(!std::is_rvalue_reference_v<Cls>, "_FunctorEnclosure: Cls cannot be r-value reference.");
	using func_signature = func_signature_t<Ret, Arg, Cls>;

	_FunctorEnclosure(func_signature function, _Storage closure) : function(function), closure(closure) {}

	Ret operator()(Arg arg) override
	{
		if constexpr (std::is_void_v<Ret>)
			function(redirect<Arg>(arg), redirect<Cls>(closure));
		else
			return function(redirect<Arg>(arg), redirect<Cls>(closure));
	}
	_FunctorInterface<Ret, Arg>* clone() override
	{
		return new _FunctorEnclosure<Ret, Arg, Cls, _Storage, true>(function, closure);
	}

	func_signature function;
	_Storage closure;
};

template<typename Ret, typename Arg, typename Cls, typename _Storage>
struct _FunctorEnclosure<Ret, Arg, Cls, _Storage, false> : public _FunctorInterface<Ret, Arg>
{
	static_assert(!std::is_rvalue_reference_v<_Storage>, "_FunctorEnclosure: _Storage cannot be r-value reference.");
	static_assert(std::is_convertible_v<_Storage, Cls>, "_FunctorEnclosure: _Storage is not convertible to Cls.");
	static_assert(!std::is_rvalue_reference_v<Cls>, "_FunctorEnclosure: Cls cannot be r-value reference.");
	using func_signature = func_signature_t<Ret, Arg, Cls>;

	_FunctorEnclosure(func_signature function, auto&& closure) : function(function), closure(forward(closure)) {}

	Ret operator()(Arg arg) override
	{
		if constexpr (std::is_void_v<Ret>)
			function(redirect<Arg>(arg), redirect<Cls>(closure));
		else
			return function(redirect<Arg>(arg), redirect<Cls>(closure));
	}
	_FunctorInterface<Ret, Arg>* clone() override
	{
		return new _FunctorEnclosure<Ret, Arg, Cls, _Storage, false>(function, std::forward<_Storage&>(closure));
	}

	func_signature function;
	_Storage closure;
};

template<typename Ret, typename Arg, typename _Storage, bool _ValueIn>
struct _FunctorEnclosure<Ret, Arg, void, _Storage, _ValueIn> : public _FunctorInterface<Ret, Arg>
{
	static_assert(std::is_void_v<_Storage>, "_FunctorEnclosure: _Storage must be void when Cls is void.");
	using func_signature = func_signature_t<Ret, Arg, void>;

	_FunctorEnclosure(func_signature function) : function(function) {}

	Ret operator()(Arg arg) override
	{
		if constexpr (std::is_void_v<Ret>)
			function(redirect<Arg>(arg));
		else
			return function(redirect<Arg>(arg));
	}
	_FunctorInterface<Ret, Arg>* clone() override
	{
		return new _FunctorEnclosure<Ret, Arg, void, void, _ValueIn>(function);
	}

	func_signature function;
};

template<typename Ret, typename Cls, typename _Storage>
struct _FunctorEnclosure<Ret, void, Cls, _Storage, true> : public _FunctorInterface<Ret, void>
{
	static_assert(!std::is_lvalue_reference_v<_Storage>, "_FunctorEnclosure: _Storage cannot be l-value reference when _ValueIn=true.");
	static_assert(!std::is_rvalue_reference_v<_Storage>, "_FunctorEnclosure: _Storage cannot be r-value reference.");
	static_assert(std::is_convertible_v<_Storage, Cls>, "_FunctorEnclosure: _Storage is not convertible to Cls.");
	static_assert(!std::is_rvalue_reference_v<Cls>, "_FunctorEnclosure: Cls cannot be r-value reference.");
	using func_signature = func_signature_t<Ret, void, Cls>;

	_FunctorEnclosure(func_signature function, _Storage closure) : function(function), closure(closure) {}

	Ret operator()() override
	{
		if constexpr (std::is_void_v<Ret>)
			function(redirect<Cls>(closure));
		else
			return function(redirect<Cls>(closure));
	}
	_FunctorInterface<Ret, void>* clone() override
	{
		return new _FunctorEnclosure<Ret, void, Cls, _Storage, true>(function, closure);
	}

	func_signature function;
	_Storage closure;
};

template<typename Ret, typename Cls, typename _Storage>
struct _FunctorEnclosure<Ret, void, Cls, _Storage, false> : public _FunctorInterface<Ret, void>
{
	static_assert(!std::is_rvalue_reference_v<_Storage>, "_FunctorEnclosure: _Storage cannot be r-value reference.");
	static_assert(std::is_convertible_v<_Storage, Cls>, "_FunctorEnclosure: _Storage is not convertible to Cls.");
	static_assert(!std::is_rvalue_reference_v<Cls>, "_FunctorEnclosure: Cls cannot be r-value reference.");
	using func_signature = func_signature_t<Ret, void, Cls>;

	_FunctorEnclosure(func_signature function, auto&& closure) : function(function), closure(forward(closure)) {}

	Ret operator()() override
	{
		if constexpr (std::is_void_v<Ret>)
			function(redirect<Cls>(closure));
		else
			return function(redirect<Cls>(closure));
	}
	_FunctorInterface<Ret, void>* clone() override
	{
		return new _FunctorEnclosure<Ret, void, Cls, _Storage, false>(function, std::forward<_Storage&>(closure));
	}

	func_signature function;
	_Storage closure;
};

template<typename Ret, typename _Storage, bool _ValueIn>
struct _FunctorEnclosure<Ret, void, void, _Storage, _ValueIn> : public _FunctorInterface<Ret, void>
{
	static_assert(std::is_void_v<_Storage>, "_FunctorEnclosure: _Storage must be void when Cls is void.");
	using func_signature = func_signature_t<Ret, void, void>;

	_FunctorEnclosure(func_signature function) : function(function) {}

	Ret operator()() override
	{
		if constexpr (std::is_void_v<Ret>)
			function();
		else
			return function();
	}
	_FunctorInterface<Ret, void>* clone() override
	{
		return new _FunctorEnclosure<Ret, void, void, void, _ValueIn>(function);
	}

	func_signature function;
};

struct null_functor_error : public std::exception
{
	null_functor_error() : std::exception("Tried to call null functor.") {}
};

template<typename Ret, typename Arg>
class Functor
{
	_FunctorInterface<Ret, Arg>* f = nullptr;

public:
	using RetType = Ret;
	using ArgType = Arg;

	template<typename Cls, typename _Storage, bool _ValueIn>
	Functor(_FunctorEnclosure<Ret, Arg, Cls, _Storage, _ValueIn>* f) : f(f) {}
	Functor(_FunctorInterface<Ret, Arg>* f) : f(f) {}
	Functor(const Functor<Ret, Arg>& other)
	{
		if (other.f)
			f = other.f->clone();
	}
	Functor(Functor<Ret, Arg>&& other) noexcept : f(other.f) { other.f = nullptr; }
	Functor& operator=(const Functor<Ret, Arg>& other)
	{
		if (this == &other)
			return *this;
		if (f)
			delete f;
		if (other.f)
			f = other.f->clone();
		else
			f = nullptr;
		return *this;
	}
	Functor& operator=(Functor&& other) noexcept
	{
		if (this == &other)
			return *this;
		if (f)
			delete f;
		f = other.f;
		other.f = nullptr;
		return *this;
	}
	~Functor() { if (f) delete f; }
	Functor<Ret, Arg> clone() const
	{
		return f ? Functor<Ret, Arg>(f->clone()) : nullptr;
	}

	Ret operator()(Arg arg) const
	{
		if constexpr (std::is_void_v<Ret>)
			(*f)(arg);
		else
			return (*f)(arg);
	}
	Ret safe_call(Arg arg) const
	{
		if (f)
		{
			if constexpr (std::is_void_v<Ret>)
				(*f)(arg);
			else
				return (*f)(arg);
		}
		else throw null_functor_error();
	}
};

template<typename Ret>
class Functor<Ret, void>
{
	_FunctorInterface<Ret, void>* f = nullptr;

public:
	using RetType = Ret;
	using ArgType = void;

	template<typename Cls, typename _Storage, bool _ValueIn>
	Functor(_FunctorEnclosure<Ret, void, Cls, _Storage, _ValueIn>* f) : f(f) {}
	Functor(_FunctorInterface<Ret, void>* f) : f(f) {}
	Functor(const Functor<Ret, void>& other)
	{
		if (other.f)
			f = other.f->clone();
	}
	Functor(Functor<Ret, void>&& other) noexcept : f(other.f) { other.f = nullptr; }
	Functor& operator=(const Functor<Ret, void>& other)
	{
		if (this == &other)
			return *this;
		if (f)
			delete f;
		if (other.f)
			f = other.f->clone();
		else
			f = nullptr;
		return *this;
	}
	Functor& operator=(Functor&& other) noexcept
	{
		if (this == &other)
			return *this;
		if (f)
			delete f;
		f = other.f;
		other.f = nullptr;
		return *this;
	}
	~Functor() { if (f) delete f; }
	Functor<Ret, void> clone() const
	{
		return f ? Functor<Ret, void>(f->clone()) : nullptr;
	}

	Ret operator()() const
	{
		if constexpr (std::is_void_v<Ret>)
			(*f)();
		else
			return (*f)();
	}
	Ret safe_call() const
	{
		if (f)
		{
			if constexpr (std::is_void_v<Ret>)
				(*f)();
			else
				return (*f)();
		}
		else throw null_functor_error();
	}
};

template<typename T>
struct is_functor_ptr : public std::false_type {};

template<typename Ret, typename Arg>
struct is_functor_ptr<Functor<Ret, Arg>> : public std::true_type {};

template<typename T>
constexpr bool is_functor_ptr_v = is_functor_ptr<T>::value;

template<typename T>
constexpr bool decays_to_functor_ptr_v = is_functor_ptr_v<std::decay_t<T>>;

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
constexpr unsigned char parse_function_v = parse_function<Lambda>::value;

// TODO rename make_functor_ptr to make_functor

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
			return Functor<Ret, Arg>(new _FunctorEnclosure<Ret, Arg, Cls, _Storage, false>(f, forward(closure)));
		}
		else
		{
			using _Storage = std::remove_reference_t<decltype(closure)>;
			return Functor<Ret, Arg>(new _FunctorEnclosure<Ret, Arg, Cls, _Storage, false>(f, forward(closure)));
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
			return Functor<Ret, void>(new _FunctorEnclosure<Ret, void, Cls, _Storage, false>(f, forward(closure)));
		}
		else
		{
			using _Storage = std::remove_reference_t<decltype(closure)>;
			return Functor<Ret, void>(new _FunctorEnclosure<Ret, void, Cls, _Storage, false>(f, forward(closure)));
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
		return Functor<Ret, Arg>(new _FunctorEnclosure<Ret, Arg, Cls, _Storage, true>(f, closure));
	}
	else
	{
		using Ret = parse_function<Func>::ret_type;
		using Cls = parse_function<Func>::par_type;
		using _Storage = decltype(closure);
		static_assert(std::is_same_v<std::decay_t<Cls>, std::decay_t<decltype(closure)>>);
		static_assert(std::is_same_v<Func, func_signature_t<Ret, void, Cls>>);
		return Functor<Ret, void>(new _FunctorEnclosure<Ret, void, Cls, _Storage, true>(f, closure));
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
		return Functor<Ret, Arg>(new _FunctorEnclosure<Ret, Arg, void, void, true>(f));
	}
	else
	{
		static_assert(std::is_same_v<Func, func_signature_t<Ret, void, void>>);
		return Functor<Ret, void>(new _FunctorEnclosure<Ret, void, void, void, true>(f));
	}
}

inline Functor<void, void> VoidFunctor = make_functor_ptr([]() {});
