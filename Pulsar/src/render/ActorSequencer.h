#pragma once

#include "Typedefs.h"
#include "ActorRenderBase.h"
#include "ActorPrimitive.h"

class ActorSequencer2D : public ActorRenderBase2D
{
	ACTOR_RENDER_INHERIT(ActorSequencer2D, ActorRenderBase2D)
private:
	friend class CanvasLayer;
	friend class ActorTesselation2D;
public:
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	virtual ActorPrimitive2D* const operator[](const int& i) = 0;
	virtual BufferCounter PrimitiveCount() const = 0;
	virtual void OnPreDraw() {}
	virtual void OnPostDraw() {}
};
