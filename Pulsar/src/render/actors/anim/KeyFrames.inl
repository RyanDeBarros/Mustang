#pragma once

#include <glm/glm.hpp>

#include "AnimationTrack.inl"

template<typename _Property>
struct KF_Assign : public KeyFrame<_Property>
{
	static constexpr bool progressive = false;
	_Property value;
	KF_Assign(float timepoint, const _Property& value) : KeyFrame<_Property>(timepoint), value(value) {}
	KF_Assign(float timepoint, _Property&& value) : KeyFrame<_Property>(timepoint), value(std::move(value)) {}
};

template<typename _Property, unsigned int _InterpMethod, bool _InOrder>
struct interpexec<_Property, KF_Assign<_Property>, _InterpMethod, _InOrder>
{
	static void _(_Property* target, KF_Assign<_Property>& a, KF_Assign<_Property>& b, float t, float period)
	{
		if (target)
			*target = interpolate<_Property, _InterpMethod, _InOrder>::_(a.value, a.timepoint, b.value, b.timepoint, t, period);
	}
};

template<typename _Property>
struct KF_Assign_Unsafe : public KeyFrame<_Property>
{
	static constexpr bool progressive = false;
	_Property value;
	KF_Assign_Unsafe(float timepoint, const _Property& value) : KeyFrame<_Property>(timepoint), value(value) {}
	KF_Assign_Unsafe(float timepoint, _Property&& value) : KeyFrame<_Property>(timepoint), value(std::move(value)) {}
};

template<typename _Property, unsigned int _InterpMethod, bool _InOrder>
struct interpexec<_Property, KF_Assign_Unsafe<_Property>, _InterpMethod, _InOrder>
{
	static void _(_Property* target, KF_Assign_Unsafe<_Property>& a, KF_Assign_Unsafe<_Property>& b, float t, float period)
	{
		*target = interpolate<_Property, _InterpMethod, _InOrder>::_(a.value, a.timepoint, b.value, b.timepoint, t, period);
	}
};

template<typename _Property>
struct KF_Event : public KeyFrame<_Property>
{
	static constexpr bool progressive = true;
	Functor<void, _Property*> event;
	KF_Event(float timepoint, const Functor<void, _Property*>& event) : KeyFrame<_Property>(timepoint), event(event) {}
	KF_Event(float timepoint, Functor<void, _Property*>&& event) : KeyFrame<_Property>(timepoint), event(std::move(event)) {}
};

template<typename _Property, unsigned int _InterpMethod, bool _InOrder>
struct interpexec<_Property, KF_Event<_Property>, _InterpMethod, _InOrder>
{
	static void _(_Property* target, KF_Event<_Property>& a, KF_Event<_Property>& b, float t, float period)
	{
		a.event(target);
	}
};

template<bool _InOrder>
struct interpolate<float, Interp::Constant, _InOrder>
{
	static float _(float a, float ta, float b, float tb, float t, float period) { return a; }
};

template<bool _InOrder>
struct interpolate<float, Interp::Linear, _InOrder>
{
	static float _(float a, float ta, float b, float tb, float t, float period)
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
	static glm::vec2 _(const glm::vec2& a, float ta, const glm::vec2& b, float tb, float t, float period)
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
	static glm::vec3 _(const glm::vec3& a, float ta, const glm::vec3& b, float tb, float t, float period)
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
	static glm::vec4 _(const glm::vec4& a, float ta, const glm::vec4& b, float tb, float t, float period)
	{
		return {
			interpolate<float, _InterpMethod, _InOrder>::_(a.x, ta, b.x, tb, t, period),
			interpolate<float, _InterpMethod, _InOrder>::_(a.y, ta, b.y, tb, t, period),
			interpolate<float, _InterpMethod, _InOrder>::_(a.z, ta, b.z, tb, t, period),
			interpolate<float, _InterpMethod, _InOrder>::_(a.w, ta, b.w, tb, t, period)
		};
	}
};
