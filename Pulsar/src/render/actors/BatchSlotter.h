#pragma once

#include <memory>

#include "render/ActorRenderBase.h"
#include "render/actors/ActorPrimitive.h"
#include "render/Renderable.h"

class BatchSlotter2D : public ActorRenderBase2D
{
	std::unordered_map<BatchModel, std::vector<std::shared_ptr<ActorPrimitive2D>>> m_Slots;

public:
	virtual void RequestDraw(CanvasLayer* canvas_layer) override;

	void Insert(const std::shared_ptr<ActorPrimitive2D>& primitive);
	bool Remove(const std::shared_ptr<ActorPrimitive2D>& primitive);
};
