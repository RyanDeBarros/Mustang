#pragma once

#include <memory>

#include "AnimationTrack.inl"

template<typename _Target>
struct AnimationPlayer
{
	std::vector<std::unique_ptr<AnimationTrackBase<_Target>>> tracks;

	inline _Target* GetTarget() { return target; }
	inline void SetTarget(_Target* t)
	{
		target = t;
		SyncTracks();
	}
	inline float GetPeriod() const { return period; }
	inline void SetPeriod(float per) { if (per > 0.0f) period = per; }
	inline float GetSpeed() const { return speed; }
	inline void SetSpeed(float sp) { if (sp >= 0.0f) speed = sp; }

	inline void SyncTracks()
	{
		for (auto& track : tracks)
			track->SyncTarget(target);
	}
	void OnUpdate();

private:
	_Target* target = nullptr;
	float period = 1.0f;
	float time = 0.0f;
	float speed = 1.0f;

public:
	bool isInReverse = false;
};

template<typename _Target>
void AnimationPlayer<_Target>::OnUpdate()
{
	if (isInReverse)
	{
		time -= speed * Pulsar::deltaDrawTime;
		for (auto& track : tracks)
			track->AdvanceBackward(time, period);
	}
	else
	{
		time += speed * Pulsar::deltaDrawTime;
		for (auto& track : tracks)
			track->AdvanceForward(time, period);
	}
	time = unsigned_fmod(time, period);
}
