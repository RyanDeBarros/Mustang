#include "AnimationPlayer.h"

#include "Pulsar.h"
#include "utils/Data.inl"
#include "utils/CommonMath.h"

void AnimationTrack::Insert(std::shared_ptr<KeyFrame>&& key_frame)
{
	insert_sorted(keyFrames, std::move(key_frame), [](const std::shared_ptr<KeyFrame>& a, const std::shared_ptr<KeyFrame>& b) -> bool { return a->timepoint < b->timepoint; });
}

// TODO this is only for constant interpolation.
void AnimationTrack::OnUpdate()
{
	time += speed * Pulsar::deltaDrawTime;
	if (time > period)
	{
		while (lastIndex < keyFrames.size())
		{
			(*keyFrames[lastIndex])();
			lastIndex++;
		}
		if (lastIndex == keyFrames.size())
			lastIndex = 0;
		time = std::fmod(time, period);
	}
	unsigned int prior_last_index = lastIndex;
	while (keyFrames[lastIndex]->timepoint <= time)
	{
		(*keyFrames[lastIndex])();
		lastIndex = (lastIndex + 1) % static_cast<unsigned int>(keyFrames.size());
		if (lastIndex == prior_last_index)
			break;
	}
}
