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
	DebugBatcher(ZIndex z = 0) : ActorRenderBase2D(z) {}
	DebugBatcher(const DebugBatcher&);
	DebugBatcher(DebugBatcher&&) noexcept;
	DebugBatcher& operator=(const DebugBatcher&);
	DebugBatcher& operator=(DebugBatcher&&) noexcept;

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	bool ChangeZIndex(const DebugModel& model, ZIndex z);
	bool ChangeZIndex(const DebugMultiPolygon::iterator& where, ZIndex z);
	void PushBack(const std::shared_ptr<DebugPolygon>& poly);
	void PushBack(std::shared_ptr<DebugPolygon>&& poly);
	void PushBackAll(const std::vector<std::shared_ptr<DebugPolygon>>& polys);
	void PushBackAll(std::vector<std::shared_ptr<DebugPolygon>>&& polys);
	bool Erase(const std::vector<std::shared_ptr<DebugPolygon>>::iterator& where);
	void EraseAll(const std::unordered_map<DebugModel, std::unordered_set<std::shared_ptr<DebugPolygon>>>& polys);
	bool Find(const std::shared_ptr<DebugPolygon>& poly, DebugMultiPolygon::iterator& where);
	size_t Size() const;

private:
	void Sort();
};
