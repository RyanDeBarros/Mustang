#pragma once

#include <memory>

#include "render/ActorRenderBase.h"
#include "render/actors/ActorPrimitive.h"
#include "render/Renderable.h"

class BatchSlotter2D : public ActorRenderBase2D
{
	std::unordered_map<BatchModel, std::vector<std::shared_ptr<ActorPrimitive2D>>> m_Slots;
	ZIndex m_Z = 0;

public:
	virtual void RequestDraw(CanvasLayer* canvas_layer) override;
	inline virtual ZIndex GetZIndex() const override { return m_Z; }
	inline virtual void SetZIndex(const ZIndex& z) override { m_Z = z; }

	void Insert(const std::shared_ptr<ActorPrimitive2D>& primitive);
	bool Remove(const std::shared_ptr<ActorPrimitive2D>& primitive);
};
