#pragma once

#include "render/ActorRenderBase.h"
#include "ActorPrimitive.h"

class ActorSequencer2D : public ActorRenderBase2D
{
public:
	ActorSequencer2D(ZIndex z = 0) : ActorRenderBase2D(z) {}

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	
	// TODO change int to something else, maybe a typedef
	virtual ActorPrimitive2D* const operator[](int i) { return nullptr; }
	virtual BufferCounter PrimitiveCount() const = 0;
	virtual void OnPreDraw() {}
	virtual void OnPostDraw() {}
};
