#pragma once

#include "Typedefs.h"

class ActorRenderBase2D
{
protected:
	ZIndex m_Z;

public:
	ActorRenderBase2D(ZIndex z = 0) { SetZIndex(z); }
	ActorRenderBase2D(const ActorRenderBase2D& other) { SetZIndex(other.GetZIndex()); }
	ActorRenderBase2D(ActorRenderBase2D&& other) noexcept { SetZIndex(other.GetZIndex()); }
	ActorRenderBase2D& operator=(const ActorRenderBase2D& other) { SetZIndex(other.GetZIndex()); return *this; }
	ActorRenderBase2D& operator=(ActorRenderBase2D&& other) noexcept { SetZIndex(other.GetZIndex()); return *this; }

	virtual void RequestDraw(class CanvasLayer* canvas_layer) = 0;
	inline virtual ZIndex GetZIndex() const { return m_Z; }
	inline virtual void SetZIndex(ZIndex z) { m_Z = z; }
};
