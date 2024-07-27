#pragma once

#include <memory>

#include "render/ActorRenderBase.h"
#include "render/actors/ActorPrimitive.h"
#include "render/Renderable.h"

class BatchSlotter2D : public ActorRenderBase2D
{
	std::unordered_map<BatchModel, std::vector<std::shared_ptr<ActorPrimitive2D>>> m_Slots;

public:
	BatchSlotter2D(ZIndex z = 0);
	BatchSlotter2D(const BatchSlotter2D&);
	BatchSlotter2D(BatchSlotter2D&&) noexcept;
	BatchSlotter2D& operator=(const BatchSlotter2D&);
	BatchSlotter2D& operator=(BatchSlotter2D&&) noexcept;

	virtual void RequestDraw(CanvasLayer* canvas_layer) override;

	void Insert(const std::shared_ptr<ActorPrimitive2D>& primitive);
	bool Remove(const std::shared_ptr<ActorPrimitive2D>& primitive);
};
