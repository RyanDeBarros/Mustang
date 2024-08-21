#pragma once

#include <glm/glm.hpp>
#include <glm/detail/qualifier.hpp>
#include <tuple>
#include <array>

#include "Functor.inl"

/// Closure: []
template<typename Arg = float>
inline Arg identity(Arg t) { return t; }

/// Closure: [constant]
template<typename Output = float, typename Input = float>
inline std::decay_t<Output> constant_func(Input t, Output closure) { return closure; }

/// Closure: [at | high | dirLR | inclusive | low]
inline float step_func(float t, const std::tuple<float, float, bool, bool, float>& closure)
{
	if (std::get<2>(closure))
		return (t < std::get<0>(closure)) || (!std::get<3>(closure) && t == std::get<0>(closure)) ? std::get<4>(closure) : std::get<1>(closure);
	else
		return (t > std::get<0>(closure)) || (!std::get<3>(closure) && t == std::get<0>(closure)) ? std::get<4>(closure) : std::get<1>(closure);
}

/// Closure: [slope | intercept]
inline float linear_func(float t, const std::tuple<float, float>& closure)
{
	return std::get<0>(closure) * t + std::get<1>(closure);
}

/// Closure: [multiplier | x_intercept | y_intercept]
inline float quadratic_func(float t, const std::tuple<float, float, float>& closure)
{
	return std::get<0>(closure) * static_cast<float>(glm::pow(t - std::get<1>(closure), 2)) + std::get<2>(closure);
}

/// Closure: [multiplier | exponent]
inline float power_func(float t, const std::tuple<float, float>& closure)
{
	return std::get<0>(closure) * static_cast<float>(glm::pow(t, std::get<1>(closure)));
}

inline bool less_than(float t, float comp)
{
	return t < comp;
}

template<glm::length_t RetL, glm::length_t InpL>
inline glm::vec<RetL, float> linear_combo(const glm::vec<InpL, float>& v, const std::array<glm::vec<RetL, float>, InpL + 1>& coeffs)
{
	glm::vec<RetL, float> combo = coeffs[0];
	//for (size_t i = 1; i < coeffs.size(); i++)
		//combo += coeffs[i] * v[i - 1];
	for (glm::length_t i = 0; i < InpL; i++)
		combo += coeffs[i + 1] * v[i];
	return combo;
}

template<glm::length_t RetL>
inline glm::vec<RetL, float> linear_combo_f(float t, const std::array<glm::vec<RetL, float>, 2>& closure)
{
	return closure[0] + t * closure[1];
}

/// Closure: [adder]
inline void op_add(float& t, float closure) { t += closure; }

/// Closure: [multiplier]
inline void op_mul(float& t, float closure) { t *= closure; }

template<typename From, typename To>
inline To cast(From t) { return static_cast<To>(std::forward<From>(t)); }

template<typename RetComp = float, typename Arg = float>
inline auto vec2_expand(auto&& fx, auto&& fy) requires (is_functor_ptr_v<decltype(fx)>&& is_functor_ptr_v<decltype(fy)>)
{
	using Ret = glm::vec<2, RetComp>;
	using fxt = std::decay_t<decltype(fx)>;
	using fyt = std::decay_t<decltype(fy)>;
	using cls = std::tuple<fxt, fyt>;
	return make_functor_ptr([](Arg arg, const cls& tuple) -> Ret {
		return Ret{ static_cast<RetComp>(std::get<0>(tuple)(static_cast<fxt::ArgType>(arg))), static_cast<RetComp>(std::get<1>(tuple)(static_cast<fyt::ArgType>(arg))) };
	}, cls(PULSAR_FORWARD(fx), PULSAR_FORWARD(fy)));
}

template<typename RetComp = float, typename ArgComp = float>
inline auto vec2_compwise(auto&& fx, auto&& fy) requires (is_functor_ptr_v<decltype(fx)> && is_functor_ptr_v<decltype(fy)>)
{
	using Ret = glm::vec<2, RetComp>;
	using fxt = std::decay_t<decltype(fx)>;
	using fyt = std::decay_t<decltype(fy)>;
	using cls = std::tuple<fxt, fyt>;
	return make_functor_ptr([](const glm::vec<2, ArgComp>& arg, const cls& tuple) -> Ret {
		return Ret{ static_cast<RetComp>(std::get<0>(tuple)(static_cast<fxt::ArgType>(arg[0]))), static_cast<RetComp>(std::get<1>(tuple)(static_cast<fyt::ArgType>(arg[1])))};
	}, cls(PULSAR_FORWARD(fx), PULSAR_FORWARD(fy)));
}

template<typename RetComp = float, typename ArgComp = float>
inline auto vec2_each(auto&& f) requires (is_functor_ptr_v<decltype(f)>)
{
	using Ret = glm::vec<2, RetComp>;
	using ft = std::decay_t<decltype(f)>;
	using cls = FunctorPtr<ft::RetType, ft::ArgType>;
	return make_functor_ptr([](const glm::vec<2, ArgComp>& arg, const cls& f) -> Ret {
		return Ret{ static_cast<RetComp>(f(static_cast<ft::ArgType>(arg[0]))), static_cast<RetComp>(f(static_cast<ft::ArgType>(arg[1]))) };
	}, PULSAR_FORWARD(f));
}

template<typename Ret, typename Arg>
inline FunctorPtr<Ret, Arg> func_sum(auto&& f1, auto&& f2) requires (is_functor_ptr_v<decltype(f1)> && is_functor_ptr_v<decltype(f2)>)
{
	using f1t = std::decay_t<decltype(f1)>;
	using f2t = std::decay_t<decltype(f2)>;
	using cls = std::tuple<f1t, f2t>;
	return make_functor_ptr([](Arg arg, const cls& tuple) -> Ret {
		return static_cast<Ret>(std::get<0>(tuple)(static_cast<f1t::ArgType>(arg)) + std::get<1>(tuple)(static_cast<f2t::ArgType>(arg)));
	}, cls(PULSAR_FORWARD(f1), PULSAR_FORWARD(f2)));
}

template<typename Ret, typename Arg>
inline FunctorPtr<Ret, Arg> func_mul(auto&& f1, auto&& f2) requires (is_functor_ptr_v<decltype(f1)>&& is_functor_ptr_v<decltype(f2)>)
{
	using f1t = std::decay_t<decltype(f1)>;
	using f2t = std::decay_t<decltype(f2)>;
	using cls = std::tuple<f1t, f2t>;
	return make_functor_ptr([](Arg arg, const cls& tuple) -> Ret {
		return static_cast<Ret>(std::get<0>(tuple)(static_cast<f1t::ArgType>(arg)) * std::get<1>(tuple)(static_cast<f2t::ArgType>(arg)));
	}, cls(PULSAR_FORWARD(f1), PULSAR_FORWARD(f2)));
}

/// f1 --> f2. Equivalently, f2(f1)
template<typename Ret, typename Arg>
inline FunctorPtr<Ret, Arg> func_compose(auto&& f1, auto&& f2) requires (is_functor_ptr_v<decltype(f1)>&& is_functor_ptr_v<decltype(f2)>)
{
	using f1t = std::decay_t<decltype(f1)>;
	using f2t = std::decay_t<decltype(f2)>;
	using cls = std::tuple<f1t, f2t>;
	return make_functor_ptr([](Arg arg, const cls& tuple) -> Ret {
		return static_cast<Ret>(std::get<1>(tuple)(static_cast<f2t::ArgType>(std::get<0>(tuple)(static_cast<f1t::ArgType>(arg)))));
	}, cls(PULSAR_FORWARD(f1), PULSAR_FORWARD(f2)));
}

template<typename Ret, typename Arg>
inline FunctorPtr<Ret, Arg> func_conditional(auto&& condition, auto&& true_case, auto&& false_case)
		requires (is_functor_ptr_v<decltype(condition)> && is_functor_ptr_v<decltype(true_case)> && is_functor_ptr_v<decltype(false_case)>)
{
	using cond_t = std::decay_t<decltype(condition)>;
	using true_t = std::decay_t<decltype(true_case)>;
	using false_t = std::decay_t<decltype(false_case)>;
	using cls = std::tuple<cond_t, true_t, false_t>;
	return make_functor_ptr([](Arg arg, const cls& tuple) -> Ret {
		if constexpr (std::is_void_v<Ret>)
		{
			if (std::get<0>(tuple)(static_cast<cond_t::ArgType>(arg)))
				std::get<1>(tuple)(static_cast<true_t::ArgType>(arg));
			else
				std::get<2>(tuple)(static_cast<false_t::ArgType>(arg));
		}
		else
		{
			if (std::get<0>(tuple)(static_cast<cond_t::ArgType>(arg)))
				return std::get<1>(tuple)(static_cast<true_t::ArgType>(arg));
			else
				return std::get<2>(tuple)(static_cast<false_t::ArgType>(arg));
		}
	}, cls(PULSAR_FORWARD(condition), PULSAR_FORWARD(true_case), PULSAR_FORWARD(false_case)));
}
