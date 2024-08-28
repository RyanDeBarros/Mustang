#pragma once

#include "transform/Fickle.inl"

typedef signed short ZIndex;

struct ActorRenderBase2D
{
	ZIndex z;
	// TODO y-sort mechanism
	ActorRenderBase2D(ZIndex z = 0) : z(z) {}
	virtual void RequestDraw(class CanvasLayer* canvas_layer) = 0;
};

struct FickleActor2D : public ActorRenderBase2D
{
	FickleActor2D(FickleType fickle_type, ZIndex z = 0) : ActorRenderBase2D(z), m_Fickler(fickle_type) {}

	inline FickleSelector2D& Fickler() { return m_Fickler; }

protected:
	FickleSelector2D m_Fickler;
};
