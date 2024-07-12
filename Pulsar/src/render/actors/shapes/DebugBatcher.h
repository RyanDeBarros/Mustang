#pragma once

#include "render/ActorRenderBase.h"

#include <unordered_map>
#include <vector>
#include <memory>

#include "render/Renderable.h"
#include "DebugMultiPolygon.h"

class DebugBatcher : public ActorRenderBase2D
{
	ZIndex m_Z;
	std::unordered_map<DebugModel, DebugMultiPolygon> m_Slots;
	std::vector<DebugMultiPolygon*> m_OrderedTraversal;

public:
	DebugBatcher(const ZIndex& z = 0) : m_Z(z) {}
	DebugBatcher(const DebugBatcher&) = delete;
	DebugBatcher(DebugBatcher&&) = delete;

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	virtual ZIndex GetZIndex() const override { return m_Z; }
	virtual void SetZIndex(const ZIndex& z) override { m_Z = z; }

	bool ChangeZIndex(const DebugModel& model, const ZIndex& z);
	bool ChangeZIndex(const DebugMultiPolygon::iterator& where, const ZIndex& z);
	void PushBack(const std::shared_ptr<DebugPolygon>& poly);
	void PushBackAll(const std::vector<std::shared_ptr<DebugPolygon>>& polys);
	bool Erase(const std::vector<std::shared_ptr<DebugPolygon>>::iterator& where);
	bool Find(const std::shared_ptr<DebugPolygon>& poly, DebugMultiPolygon::iterator& where);

private:
	void Sort();
};
