#pragma once

#include <vector>
#include <type_traits>

#include "Pulsar.h"
#include "utils/CommonMath.h"
#include "utils/Data.inl"
#include "utils/Functor.inl"

template<typename _Property>
struct KeyFrame
{
	static constexpr bool progressive = false;
	float timepoint;
	inline KeyFrame(float timepoint) : timepoint(timepoint) {}
};

namespace Interp {
	constexpr unsigned int Constant = 0;
	constexpr unsigned int Linear = 1;
}

template<typename _Value, unsigned int _InterpMethod, bool _InOrder>
struct interpolate
{
	static_assert(false, "interpolate is not defined for type and/or interpolation method.");
	inline static std::decay_t<_Value> _(_Value a, float ta, _Value b, float tb, float t, float period) {}
};

template<typename _Property, typename _KeyFrame, unsigned int _InterpMethod, bool _InOrder>
struct interpexec
{
	static_assert(false, "interpexec is not defined for keyframe type and/or interpolation method.");
	inline static void _(_Property* property, _KeyFrame& a, _KeyFrame& b, float t, float period) {}
};

template<typename _Target>
struct AnimationTrackBase
{
	virtual void SyncTarget(_Target* target) = 0;
	virtual void AdvanceForward(float time, float period) = 0;
	virtual void AdvanceBackward(float time, float period) = 0;
};

template<typename _Target>
struct AnimationPlayer;

/// Progressive track executes every keyframe (ex: event track). Non-progressive track only executes interpolated keyframe (ex: property callback track).
/// As a corollary, progressive track interpolation does not depend on time - i.e., constant.
template<typename _Target, typename _Property, std::derived_from<KeyFrame<_Property>> _KeyFrame, unsigned int _InterpMethod>
struct AnimationTrack : public AnimationTrackBase<_Target>
{
	static_assert(!_KeyFrame::progressive || _InterpMethod == Interp::Constant, "Progressive animation track must only use constant interpolation method.");

	static constexpr auto comparator = [](const _KeyFrame& a, const _KeyFrame& b) -> bool { return a.timepoint < b.timepoint; };
	static constexpr auto equalitor = [](const _KeyFrame& a, const _KeyFrame& b) -> bool { return a.timepoint == b.timepoint; };

	std::vector<_KeyFrame> keyFrames;
	FunctorPtr<void, void> callback = VoidFunctorPtr;
	FunctorPtr<_Property*, _Target*> getProperty = make_functor_ptr([](_Target*) -> _Property* { return nullptr; });

	inline AnimationTrack() = default;
	
	void Insert(const _KeyFrame& key_frame);
	void Insert(_KeyFrame&& key_frame);
	void SetOrInsert(const _KeyFrame& key_frame);
	void SetOrInsert(_KeyFrame&& key_frame);

private:
	friend struct AnimationPlayer<_Target>;
	_Property* property = nullptr;
	unsigned short lastIndex = 0;

	inline void SyncTarget(_Target* target) override { property = getProperty(target); }
	void AdvanceForward(float time, float period) override;
	void AdvanceBackward(float time, float period) override;

};

template<typename _Target, typename _Property, std::derived_from<KeyFrame<_Property>> _KeyFrame, unsigned int _InterpMethod>
inline void AnimationTrack<_Target, _Property, _KeyFrame, _InterpMethod>::Insert(const _KeyFrame& key_frame)
{
	insert_sorted(keyFrames, key_frame, comparator);
}

template<typename _Target, typename _Property, std::derived_from<KeyFrame<_Property>> _KeyFrame, unsigned int _InterpMethod>
inline void AnimationTrack<_Target, _Property, _KeyFrame, _InterpMethod>::Insert(_KeyFrame&& key_frame)
{
	insert_sorted(keyFrames, std::move(key_frame), comparator);
}

template<typename _Target, typename _Property, std::derived_from<KeyFrame<_Property>> _KeyFrame, unsigned int _InterpMethod>
inline void AnimationTrack<_Target, _Property, _KeyFrame, _InterpMethod>::SetOrInsert(const _KeyFrame& key_frame)
{
	set_or_insert_sorted(keyFrames, key_frame, equalitor, comparator);
}

template<typename _Target, typename _Property, std::derived_from<KeyFrame<_Property>> _KeyFrame, unsigned int _InterpMethod>
inline void AnimationTrack<_Target, _Property, _KeyFrame, _InterpMethod>::SetOrInsert(_KeyFrame&& key_frame)
{
	set_or_insert_sorted(keyFrames, std::move(key_frame), equalitor, comparator);
}

template<typename _Target, typename _Property, std::derived_from<KeyFrame<_Property>> _KeyFrame, unsigned int _InterpMethod>
inline void AnimationTrack<_Target, _Property, _KeyFrame, _InterpMethod>::AdvanceForward(float time, float period)
{
	if (keyFrames.empty())
		return;
	if constexpr (_KeyFrame::progressive)
	{
		static constexpr unsigned char loopLimit = 3;
		unsigned char loopCount = 1;
		while (keyFrames[lastIndex].timepoint <= time)
		{
			unsigned short nextIndex = (lastIndex + 1) % keyFrames.size();
			if (nextIndex == 0)
				interpexec<_Property, _KeyFrame, _InterpMethod, false>::_(property, keyFrames[lastIndex], keyFrames[nextIndex], 0, 0);
			else
				interpexec<_Property, _KeyFrame, _InterpMethod, true>::_(property, keyFrames[lastIndex], keyFrames[nextIndex], 0, 0);
			lastIndex = nextIndex;
			if (lastIndex == 0)
			{
				if (time >= period)
				{
					loopCount++;
					time -= period;
					if (loopCount >= loopLimit)
					{
						time = unsigned_fmod(time, period);
						break;
					}
				}
				else
					break;
			}
		}
	}
	else
	{
		time = unsigned_fmod(time, period);
		if (keyFrames[lastIndex].timepoint > time)
		{
			lastIndex = 0;
			if (keyFrames[lastIndex].timepoint > time)
			{
				interpexec<_Property, _KeyFrame, _InterpMethod, false>::_(property, keyFrames[keyFrames.size() - 1], keyFrames[0], time + period, period);
				return;
			}
		}
		unsigned short nextIndex = lastIndex + 1;
		while (nextIndex < keyFrames.size() && keyFrames[nextIndex].timepoint <= time)
			nextIndex++;
		if (nextIndex == keyFrames.size()) [[unlikely]]
		{
			lastIndex = 0;
			interpexec<_Property, _KeyFrame, _InterpMethod, false>::_(property, keyFrames[keyFrames.size() - 1], keyFrames[0], time, period);
		}
		else [[likely]]
		{
			lastIndex = nextIndex;
			interpexec<_Property, _KeyFrame, _InterpMethod, true>::_(property, keyFrames[lastIndex - 1], keyFrames[lastIndex], time, period);
		}
	}
	callback();
}

template<typename _Target, typename _Property, std::derived_from<KeyFrame<_Property>> _KeyFrame, unsigned int _InterpMethod>
inline void AnimationTrack<_Target, _Property, _KeyFrame, _InterpMethod>::AdvanceBackward(float time, float period)
{
	if (keyFrames.empty())
		return;
	if constexpr (_KeyFrame::progressive)
	{
		static constexpr unsigned char loopLimit = 3;
		unsigned char loopCount = 1;
		while (keyFrames[lastIndex].timepoint <= time)
		{
			unsigned short nextIndex = (lastIndex + 1) % keyFrames.size();
			if (nextIndex == 0)
				interpexec<_Property, _KeyFrame, _InterpMethod, false>::_(property, keyFrames[lastIndex], keyFrames[nextIndex], 0, 0);
			else
				interpexec<_Property, _KeyFrame, _InterpMethod, true>::_(property, keyFrames[lastIndex], keyFrames[nextIndex], 0, 0);
			lastIndex = nextIndex;
			if (lastIndex == 0)
			{
				if (time >= period)
				{
					loopCount++;
					time -= period;
					if (loopCount >= loopLimit)
					{
						time = unsigned_fmod(time, period);
						break;
					}
				}
				else
					break;
			}
		}
	}
	else
	{
		time = unsigned_fmod(time, period);
		if (keyFrames[lastIndex].timepoint < time)
		{
			lastIndex = static_cast<unsigned short>(keyFrames.size() - 1);
			if (keyFrames[lastIndex].timepoint < time)
			{
				interpexec<_Property, _KeyFrame, _InterpMethod, false>::_(property, keyFrames[keyFrames.size() - 1], keyFrames[0], time, period);
				return;
			}
		}
		short nextIndex = lastIndex - 1;
		while (nextIndex >= 0 && keyFrames[nextIndex].timepoint >= time)
			nextIndex--;
		if (nextIndex == -1) [[unlikely]]
		{
			lastIndex = static_cast<unsigned short>(keyFrames.size() - 1);
			interpexec<_Property, _KeyFrame, _InterpMethod, false>::_(property, keyFrames[keyFrames.size() - 1], keyFrames[0], time + period, period);
		}
		else [[likely]]
		{
			lastIndex = nextIndex;
			interpexec<_Property, _KeyFrame, _InterpMethod, true>::_(property, keyFrames[lastIndex], keyFrames[lastIndex + 1], time, period);
		}
	}
	callback();
}
