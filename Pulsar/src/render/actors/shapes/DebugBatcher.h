#pragma once

#include "render/ActorRenderBase.h"

#include <unordered_map>
#include <vector>
#include <memory>

#include "render/Renderable.h"
#include "DebugMultiPolygon.h"

typedef std::pair<GLenum, BatchModel> DebugModel;

template<>
struct std::hash<DebugModel>
{
	inline size_t operator()(const DebugModel& model) const
	{
		auto hash1 = hash<GLenum>{}(model.first);
		auto hash2 = hash<BatchModel>{}(model.second);
		return hash1 ^ (hash2 << 1);
	}
};

class DebugBatcher : public ActorRenderBase2D
{
	std::unordered_map<DebugModel, DebugMultiPolygon> m_Slots;

public:
	DebugBatcher(const DebugBatcher&) = delete;
	DebugBatcher(DebugBatcher&&) = delete;

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	virtual ZIndex GetZIndex() const override { return 0; }
	virtual void SetZIndex(const ZIndex& z) override {}

	bool ChangeZIndex(const std::vector<std::shared_ptr<DebugPolygon>>::iterator& where, const ZIndex& z);
	void PushBack(const std::shared_ptr<DebugPolygon>& poly);
	void PushBackAll(const std::vector<std::shared_ptr<DebugPolygon>>& polys);
	bool Erase(const std::vector<std::shared_ptr<DebugPolygon>>::iterator& where);
	bool Find(const std::shared_ptr<DebugPolygon>& poly, std::vector<std::shared_ptr<DebugPolygon>>::iterator& where);
};
