#pragma once

#include <unordered_set>

#include "../ActorRenderBase.h"

struct YSortNotification;

class YSorter : public ActorRenderBase2D
{
	std::vector<std::pair<FickleActor2D*, YSortNotification*>> actors;

public:
	YSorter(ZIndex z = 0) : ActorRenderBase2D(z) {}
	YSorter(const YSorter&) = delete;
	YSorter(YSorter&&) = delete;
	~YSorter();

	void RequestDraw(class CanvasLayer* canvas_layer) override;

	void PushBack(FickleActor2D* actor);
	void PushBackAll(const std::vector<FickleActor2D*>& actors);
	void Erase(FickleActor2D* actor);
	void EraseAll(const std::unordered_set<FickleActor2D*>& actors);

private:
	friend struct YSortNotification;
	void YSort();
	YSortNotification* AttachActor(FickleActor2D*);
	void DetachActor(std::pair<FickleActor2D*, YSortNotification*>&) const;
};

struct YSortNotification : FickleNotification
{
	YSorter* sorter;
	FickleNotification* old_notify;

	YSortNotification(YSorter* sorter, FickleNotification* old_notify) : sorter(sorter), old_notify(old_notify) {}

	void Notify(FickleSyncCode code) override
	{
		old_notify->Notify(code);
		if (code == FickleSyncCode::SyncAll || code == FickleSyncCode::SyncT || code == FickleSyncCode::SyncP)
			sorter->YSort();
	}
};
