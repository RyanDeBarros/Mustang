#pragma once

#include <vector>
#include <memory>

#include "utils/Functor.inl"

struct KeyFrame
{
	inline virtual void operator()() = 0;
	float timepoint;
	inline KeyFrame(float timepoint) : timepoint(timepoint) {}
};

struct AnimationTrack
{
	std::vector<std::shared_ptr<KeyFrame>> keyFrames;

	AnimationTrack(float period = 1.0f) { SetPeriod(period); }
	inline AnimationTrack(std::vector<std::shared_ptr<KeyFrame>>&& sorted_key_frames, float period = 1.0f) : keyFrames(std::move(sorted_key_frames)) { SetPeriod(period); }
	
	inline float GetPeriod() const { return period; }
	inline void SetPeriod(float per) { if (per > 0.0f) period = per; }
	inline float GetSpeed() const { return speed; }
	inline void SetSpeed(float sp) { if (sp > 0.0f) speed = sp; }
	void Insert(std::shared_ptr<KeyFrame>&& key_frame);
	void OnUpdate();

private:
	float period = 1.0f;
	float time = 0.0f;
	unsigned int lastIndex = 0;
	float speed = 1.0f;
	// TODO implement reverse
	bool isInReverse = false;
};
