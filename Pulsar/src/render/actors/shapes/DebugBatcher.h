#pragma once

#include "render/ActorRenderBase.h"

#include <unordered_map>

#include "render/Renderable.h"
#include "DebugMultiPolygon.h"

class DebugBatcher : public ActorRenderBase2D
{
	std::unordered_map<std::pair<GLenum, BatchModel>, DebugMultiPolygon> m_Slots;

public:
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	virtual ZIndex GetZIndex() const override { return 0; }
	virtual void SetZIndex(const ZIndex& z) override {}
};
