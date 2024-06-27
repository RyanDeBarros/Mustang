#pragma once

#include "Typedefs.h"
#include "ActorRenderBase.h"
#include "ActorPrimitive.h"

class ActorSequencer2D : public ActorRenderBase2D
{
public:
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
};
