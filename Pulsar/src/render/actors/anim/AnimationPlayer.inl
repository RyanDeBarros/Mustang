#pragma once

#include "AnimationTrack.inl"
#include "utils/CopyPtr.inl"

template<typename _Target>
struct AnimationPlayer
{
	std::vector<CopyPtr<AnimationTrackBase<_Target>>> tracks;

	AnimationPlayer() = default;
	AnimationPlayer(const AnimationPlayer<_Target>& other)
		: tracks(other.tracks), target(other.target), period(other.period), time(other.time), speed(other.speed), isInReverse(other.isInReverse)
	{
		SyncTracks();
	}
	AnimationPlayer(AnimationPlayer<_Target>&& other) noexcept
		: tracks(std::move(other.tracks)), target(other.target), period(other.period), time(other.time), speed(other.speed), isInReverse(other.isInReverse)
	{
		SyncTracks();
	}
	AnimationPlayer<_Target>& operator=(const AnimationPlayer<_Target>& other)
	{
		if (this == &other)
			return *this;
		tracks = other.tracks;
		target = other.target;
		period = other.period;
		time = other.time;
		speed = other.speed;
		isInReverse = other.isInReverse;
		SyncTracks();
		return *this;
	}
	AnimationPlayer<_Target>& operator=(AnimationPlayer<_Target>&& other) noexcept
	{
		if (this == &other)
			return *this;
		tracks = std::move(other.tracks);
		target = other.target;
		period = other.period;
		time = other.time;
		speed = other.speed;
		isInReverse = other.isInReverse;
		SyncTracks();
		return *this;
	}

	_Target* GetTarget() { return target; }
	void SetTarget(_Target* t)
	{
		target = t;
		SyncTracks();
	}
	float GetPeriod() const { return period; }
	void SetPeriod(float per) { if (per > 0.0f) period = per; }
	float GetSpeed() const { return speed; }
	void SetSpeed(float sp) { if (sp >= 0.0f) speed = sp; }

	void SyncTracks()
	{
		for (const auto& track : tracks)
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
inline void AnimationPlayer<_Target>::OnUpdate()
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
