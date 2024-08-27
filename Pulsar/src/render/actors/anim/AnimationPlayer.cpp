#include "AnimationPlayer.h"

void AnimationPlayer::OnUpdate()
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
