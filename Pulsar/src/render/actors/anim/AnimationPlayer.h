#pragma once

#include <memory>

#include "AnimationTrack.inl"

struct AnimationPlayer
{
	std::vector<std::unique_ptr<AnimationTrackBase>> tracks;

	inline float GetPeriod() const { return period; }
	inline void SetPeriod(float per) { if (per > 0.0f) period = per; }
	inline float GetSpeed() const { return speed; }
	inline void SetSpeed(float sp) { if (sp >= 0.0f) speed = sp; }

	void OnUpdate();

private:
	float period = 1.0f;
	float time = 0.0f;
	float speed = 1.0f;

public:
	bool isInReverse = false;
};
