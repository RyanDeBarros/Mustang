#pragma once

#include "Typedefs.h"

class ActorRenderBase2D
{
protected:
	ZIndex m_Z;

public:
	ActorRenderBase2D(const ZIndex& z) { SetZIndex(z); }

	virtual void RequestDraw(class CanvasLayer* canvas_layer) = 0;
	inline virtual ZIndex GetZIndex() const { return m_Z; }
	inline virtual void SetZIndex(const ZIndex& z) { m_Z = z; }
};
