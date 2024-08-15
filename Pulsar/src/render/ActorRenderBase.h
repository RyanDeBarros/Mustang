#pragma once

#include "Typedefs.h"
#include "transform/Wrapper.h"

struct ActorRenderBase2D
{
	ZIndex z;
	ActorRenderBase2D(ZIndex z = 0) : z(z) {}
	virtual void RequestDraw(class CanvasLayer* canvas_layer) = 0;
};

struct TransformableActor2D : public ActorRenderBase2D, public Transformable2D
{
	TransformableActor2D(ZIndex z = 0, const Transform2D& transform = {}) : ActorRenderBase2D(z), Transformable2D(transform) {}
};

struct ModulatableActor2D : public ActorRenderBase2D, public Modulatable
{
	ModulatableActor2D(ZIndex z = 0, const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f }) : ActorRenderBase2D(z), Modulatable(modulate) {}
};

struct ProteanActor2D : public ActorRenderBase2D, public Protean2D
{
	ProteanActor2D(ZIndex z = 0, const Transform2D& transform = {}, const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f }) : ActorRenderBase2D(z), Protean2D(transform, modulate) {}
};
