#include "YSorter.h"

#include "../CanvasLayer.h"
#include "utils/Data.inl"

static constexpr auto ysort_compare = [](const std::pair<FickleActor2D*, YSortNotification*>& a, const std::pair<FickleActor2D*, YSortNotification*>& b) -> bool
	{ return a.first->Fickler().PackedP()->y < b.first->Fickler().PackedP()->y; };

YSorter::~YSorter()
{
	for (auto& pair : actors)
		DetachActor(pair);
}

void YSorter::RequestDraw(CanvasLayer* canvas_layer)
{
	for (auto it = actors.begin(); it != actors.end(); ++it)
		it->first->RequestDraw(canvas_layer);
}

void YSorter::PushBack(FickleActor2D* actor)
{
	insert_sorted(actors, { actor, AttachActor(actor) }, ysort_compare);
}

void YSorter::PushBackAll(const std::vector<FickleActor2D*>& new_actors)
{
	for (auto actor : new_actors)
		PushBack(actor);
}

void YSorter::Erase(FickleActor2D* actor)
{
	actors.erase(std::remove_if(actors.begin(), actors.end(), [actor, this](std::pair<FickleActor2D*, YSortNotification*>& pair) {
		if (pair.first == actor)
		{
			DetachActor(pair);
			return true;
		}
		else return false;
	}));
}

void YSorter::EraseAll(const std::unordered_set<FickleActor2D*>& old_actors)
{
	actors.erase(std::remove_if(actors.begin(), actors.end(), [old_actors, this](std::pair<FickleActor2D*, YSortNotification*>& pair) {
		if (old_actors.find(pair.first) != old_actors.end())
		{
			DetachActor(pair);
			return true;
		}
		else return false;
	}));
}

void YSorter::YSort()
{
	insertion_sort(actors, ysort_compare);
}

YSortNotification* YSorter::AttachActor(FickleActor2D* actor)
{
	auto old_notify = actor->Fickler().Transformer()->notify;
	YSortNotification* new_notify = new YSortNotification(this, old_notify);
	actor->Fickler().Transformer()->notify = new_notify;
	return new_notify;
}

void YSorter::DetachActor(std::pair<FickleActor2D*, YSortNotification*>& pair) const
{
	if (pair.second)
	{
		pair.first->Fickler().Transformer()->notify = pair.second->old_notify;
		delete pair.second;
		pair.second = nullptr;
	}
}
