#pragma once

#include "render/ActorRenderBase.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

#include "render/Renderable.h"
#include "DebugMultiPolygon.h"

class DebugBatcher : public ActorRenderBase2D
{
	std::unordered_map<DebugModel, DebugMultiPolygon> m_Slots;
	std::vector<DebugMultiPolygon*> m_OrderedTraversal;

public:
	DebugBatcher(const ZIndex& z = 0) : ActorRenderBase2D(z) {}
	DebugBatcher(const DebugBatcher&) = delete;
	DebugBatcher(DebugBatcher&&) = delete;

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	bool ChangeZIndex(const DebugModel& model, const ZIndex& z);
	bool ChangeZIndex(const DebugMultiPolygon::iterator& where, const ZIndex& z);
	void PushBack(const std::shared_ptr<DebugPolygon>& poly);
	void PushBackAll(const std::vector<std::shared_ptr<DebugPolygon>>& polys);
	bool Erase(const std::vector<std::shared_ptr<DebugPolygon>>::iterator& where);
	void EraseAll(const std::unordered_map<DebugModel, std::unordered_set<std::shared_ptr<DebugPolygon>>>& polys);
	bool Find(const std::shared_ptr<DebugPolygon>& poly, DebugMultiPolygon::iterator& where);

private:
	void Sort();
};
