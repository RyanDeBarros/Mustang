#pragma once

#include "Typedefs.h"
#include "transform/Protean.h"

struct ActorRenderBase2D
{
	ZIndex z;
	ActorRenderBase2D(ZIndex z = 0) : z(z) {}
	virtual void RequestDraw(class CanvasLayer* canvas_layer) = 0;
};

class ProteanActor2D : public ActorRenderBase2D, public Protean2D
{
public:
	ProteanActor2D(ZIndex z = 0, const Transform2D& transform = {}, const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f }) : ActorRenderBase2D(z), Protean2D(transform, modulate) {}
};
