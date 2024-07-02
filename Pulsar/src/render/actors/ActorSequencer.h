#pragma once

#include "render/ActorRenderBase.h"
#include "ActorPrimitive.h"

class ActorSequencer2D : public ActorRenderBase2D
{
public:
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	virtual ActorPrimitive2D* const operator[](const int& i) { return nullptr; }
	virtual BufferCounter PrimitiveCount() const = 0;
	virtual void OnPreDraw() {}
	virtual void OnPostDraw() {}
};
