#pragma once

#include <vector>
#include <type_traits>

#include "Pulsar.h"
#include "utils/CommonMath.h"
#include "utils/Data.inl"
#include "utils/Functor.inl"

template<typename _Target>
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
	inline static std::decay_t<_Value> _(_Value a, float ta, _Value b, float tb, float t) {}
};

template<typename _Target, typename _KeyFrame, unsigned int _InterpMethod, bool _InOrder>
struct interpexec
{
	static_assert(false, "interpexec is not defined for keyframe type and/or interpolation method.");
	inline static void _(_Target* target, _KeyFrame& a, _KeyFrame& b, float t) {}
};

/// Progressive track executes every keyframe (ex: event track). Non-progressive track only executes interpolated keyframe (ex: property callback track).
template<typename _Target, std::derived_from<KeyFrame<_Target>> _KeyFrame>
struct AnimationTrack
{
	static constexpr auto comparator = [](const _KeyFrame& a, const _KeyFrame& b) -> bool { return a.timepoint < b.timepoint; };
	static constexpr auto equalitor = [](const _KeyFrame& a, const _KeyFrame& b) -> bool { return a.timepoint == b.timepoint; };

	std::vector<_KeyFrame> keyFrames;
	FunctorPtr<void, void> callback = VoidFunctorPtr;
	_Target* target = nullptr;

	inline AnimationTrack(float period = 1.0f) { SetPeriod(period); }
	inline AnimationTrack(const std::vector<_KeyFrame>& sorted_key_frames, float period = 1.0f) : keyFrames(sorted_key_frames) { SetPeriod(period); }
	inline AnimationTrack(std::vector<_KeyFrame>&& sorted_key_frames, float period = 1.0f) : keyFrames(std::move(sorted_key_frames)) { SetPeriod(period); }
	
	inline float GetPeriod() const { return period; }
	inline void SetPeriod(float per) { if (per > 0.0f) period = per; }
	inline float GetSpeed() const { return speed; }
	inline void SetSpeed(float sp) { if (sp > 0.0f) speed = sp; }
	void Insert(const _KeyFrame& key_frame);
	void Insert(_KeyFrame&& key_frame);
	void SetOrInsert(const _KeyFrame& key_frame);
	void SetOrInsert(_KeyFrame&& key_frame);
	void OnUpdate();

private:
	float period = 1.0f;
	float time = 0.0f;
	unsigned int lastIndex = 0;
	float speed = 1.0f;
	// TODO implement reverse
	bool isInReverse = false;

	void Interpolate(float newtime);
};

template<typename _Target, std::derived_from<KeyFrame<_Target>> _KeyFrame>
void AnimationTrack<_Target, _KeyFrame>::Insert(const _KeyFrame& key_frame)
{
	insert_sorted(keyFrames, key_frame, comparator);
}

template<typename _Target, std::derived_from<KeyFrame<_Target>> _KeyFrame>
void AnimationTrack<_Target, _KeyFrame>::Insert(_KeyFrame&& key_frame)
{
	insert_sorted(keyFrames, std::move(key_frame), comparator);
}

template<typename _Target, std::derived_from<KeyFrame<_Target>> _KeyFrame>
void AnimationTrack<_Target, _KeyFrame>::SetOrInsert(const _KeyFrame& key_frame)
{
	set_or_insert_sorted(keyFrames, key_frame, equalitor, comparator);
}

template<typename _Target, std::derived_from<KeyFrame<_Target>> _KeyFrame>
void AnimationTrack<_Target, _KeyFrame>::SetOrInsert(_KeyFrame&& key_frame)
{
	set_or_insert_sorted(keyFrames, std::move(key_frame), equalitor, comparator);
}

template<typename _Target, std::derived_from<KeyFrame<_Target>> _KeyFrame>
void AnimationTrack<_Target, _KeyFrame>::OnUpdate()
{
	Interpolate(time + Pulsar::deltaDrawTime);
	callback();
}

template<typename _Target, std::derived_from<KeyFrame<_Target>> _KeyFrame>
void AnimationTrack<_Target, _KeyFrame>::Interpolate(float newtime)
{
	// TODO in progressive/non-progressive, handle keyFrames.size() == 0 or 1.
	if constexpr (_KeyFrame::progressive)
	{
		static constexpr unsigned int loopLimit = 3;
		unsigned int loopCount = 1;
		while (keyFrames[lastIndex].timepoint <= newtime)
		{
			unsigned int nextIndex = (lastIndex + 1) % keyFrames.size();
			// TODO use general method
			if (nextIndex == 0)
				interpexec<_Target, _KeyFrame, Interp::Linear, false>::_(target, keyFrames[lastIndex], keyFrames[nextIndex], time);
			else
				interpexec<_Target, _KeyFrame, Interp::Linear, true>::_(target, keyFrames[lastIndex], keyFrames[nextIndex], time);
			lastIndex = nextIndex;
			if (lastIndex == 0)
			{
				if (newtime >= period)
				{
					loopCount++;
					newtime -= period;
					if (loopCount >= loopLimit)
					{
						newtime = unsigned_fmod(newtime, period);
						break;
					}
				}
				else
					break;
			}
		}
		time = newtime;
	}
	else
	{
		time = unsigned_fmod(newtime, period);
		if (keyFrames[lastIndex].timepoint > time)
		{
			lastIndex = 0;
			if (keyFrames[lastIndex].timepoint > time)
			{
				// TODO use general method
				interpexec<_Target, _KeyFrame, Interp::Linear, false>::_(target, keyFrames[keyFrames.size() - 1], keyFrames[0], time);
				return;
			}
		}
		unsigned int nextIndex = lastIndex + 1;
		while (nextIndex < keyFrames.size() && keyFrames[nextIndex].timepoint <= time)
			nextIndex++;
		if (nextIndex == keyFrames.size()) [[unlikely]]
		{
			lastIndex = 0;
			// TODO use general method
			interpexec<_Target, _KeyFrame, Interp::Linear, false>::_(target, keyFrames[keyFrames.size() - 1], keyFrames[0], time);
		}
		else [[likely]]
		{
			lastIndex = nextIndex;
			// TODO use general method
			interpexec<_Target, _KeyFrame, Interp::Linear, true>::_(target, keyFrames[lastIndex - 1], keyFrames[lastIndex], time);
		}
	}
}
