#pragma once

#include "Typedefs.h"

class ActorRenderBase2D
{
public:
	virtual void RequestDraw(class CanvasLayer* canvas_layer) = 0;
	virtual BufferCounter PrimitiveCount() const = 0;
	virtual class ActorPrimitive2D* const operator[](const int& i) = 0;
	virtual void OnPreDraw() {}
	virtual void OnPostDraw() {}
	virtual ZIndex GetZIndex() const { return 0; }
	virtual void SetZIndex(const ZIndex& z) {}
};
