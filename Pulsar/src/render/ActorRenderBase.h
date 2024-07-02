#pragma once

#include "Typedefs.h"

class ActorRenderBase2D
{
public:
	virtual void RequestDraw(class CanvasLayer* canvas_layer) = 0;
	virtual ZIndex GetZIndex() const { return 0; }
	virtual void SetZIndex(const ZIndex& z) {}
};
