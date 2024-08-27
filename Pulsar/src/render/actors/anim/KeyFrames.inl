#pragma once

#include <glm/glm.hpp>

#include "AnimationPlayer.h"

template<typename _Target>
struct KF_Assign : public KeyFrame<_Target>
{
	static constexpr bool progressive = false;
	_Target value;
	inline KF_Assign(float timepoint, const _Target& value) : KeyFrame<_Target>(timepoint), value(value) {}
	inline KF_Assign(float timepoint, _Target&& value) : KeyFrame<_Target>(timepoint), value(std::move(value)) {}
};

template<typename _Target, unsigned int _InterpMethod, bool _InOrder>
struct interpexec<_Target, KF_Assign<_Target>, _InterpMethod, _InOrder>
{
	inline static void _(_Target* target, KF_Assign<_Target>& a, KF_Assign<_Target>& b, float t, float period)
	{
		if (target)
			*target = interpolate<_Target, _InterpMethod, _InOrder>::_(a.value, a.timepoint, b.value, b.timepoint, t, period);
	}
};

template<typename _Target>
struct KF_Assign_Unsafe : public KeyFrame<_Target>
{
	static constexpr bool progressive = false;
	_Target value;
	inline KF_Assign_Unsafe(float timepoint, const _Target& value) : KeyFrame<_Target>(timepoint), value(value) {}
	inline KF_Assign_Unsafe(float timepoint, _Target&& value) : KeyFrame<_Target>(timepoint), value(std::move(value)) {}
};

template<typename _Target, unsigned int _InterpMethod, bool _InOrder>
struct interpexec<_Target, KF_Assign_Unsafe<_Target>, _InterpMethod, _InOrder>
{
	inline static void _(_Target* target, KF_Assign_Unsafe<_Target>& a, KF_Assign_Unsafe<_Target>& b, float t, float period)
	{
		*target = interpolate<_Target, _InterpMethod, _InOrder>::_(a.value, a.timepoint, b.value, b.timepoint, t, period);
	}
};

template<typename _Target>
struct KF_Event : public KeyFrame<_Target>
{
	static constexpr bool progressive = true;
	FunctorPtr<void, _Target*> event;
	inline KF_Event(float timepoint, const FunctorPtr<void, _Target*>& event) : KeyFrame<_Target>(timepoint), event(event) {}
	inline KF_Event(float timepoint, FunctorPtr<void, _Target*>&& event) : KeyFrame<_Target>(timepoint), event(std::move(event)) {}
};

template<typename _Target, unsigned int _InterpMethod, bool _InOrder>
struct interpexec<_Target, KF_Event<_Target>, _InterpMethod, _InOrder>
{
	inline static void _(_Target* target, KF_Event<_Target>& a, KF_Event<_Target>& b, float t, float period)
	{
		a.event(target);
	}
};

template<bool _InOrder>
struct interpolate<float, Interp::Constant, _InOrder>
{
	inline static float _(float a, float ta, float b, float tb, float t, float period) { return a; }
};

template<bool _InOrder>
struct interpolate<float, Interp::Linear, _InOrder>
{
	inline static float _(float a, float ta, float b, float tb, float t, float period)
	{
		if constexpr (_InOrder)
		{
			if (ta == tb) [[unlikely]]
				return a;
			else [[likely]]
				return (t - ta) * (b - a) / (tb - ta) + a;
		}
		else
			return (t - ta) * (b - a) / (period + tb - ta) + a;
	}
};

template<unsigned int _InterpMethod, bool _InOrder>
struct interpolate<glm::vec2, _InterpMethod, _InOrder>
{
	inline static glm::vec2 _(const glm::vec2& a, float ta, const glm::vec2& b, float tb, float t, float period)
	{
		return {
			interpolate<float, _InterpMethod, _InOrder>::_(a.x, ta, b.x, tb, t, period),
			interpolate<float, _InterpMethod, _InOrder>::_(a.y, ta, b.y, tb, t, period)
		};
	}
};

template<unsigned int _InterpMethod, bool _InOrder>
struct interpolate<glm::vec3, _InterpMethod, _InOrder>
{
	inline static glm::vec3 _(const glm::vec3& a, float ta, const glm::vec3& b, float tb, float t, float period)
	{
		return {
			interpolate<float, _InterpMethod, _InOrder>::_(a.x, ta, b.x, tb, t, period),
			interpolate<float, _InterpMethod, _InOrder>::_(a.y, ta, b.y, tb, t, period),
			interpolate<float, _InterpMethod, _InOrder>::_(a.z, ta, b.z, tb, t, period)
		};
	}
};

template<unsigned int _InterpMethod, bool _InOrder>
struct interpolate<glm::vec4, _InterpMethod, _InOrder>
{
	inline static glm::vec4 _(const glm::vec4& a, float ta, const glm::vec4& b, float tb, float t, float period)
	{
		return {
			interpolate<float, _InterpMethod, _InOrder>::_(a.x, ta, b.x, tb, t, period),
			interpolate<float, _InterpMethod, _InOrder>::_(a.y, ta, b.y, tb, t, period),
			interpolate<float, _InterpMethod, _InOrder>::_(a.z, ta, b.z, tb, t, period),
			interpolate<float, _InterpMethod, _InOrder>::_(a.w, ta, b.w, tb, t, period)
		};
	}
};
