#pragma once

#include <vector>

#include "render/ActorRenderBase.h"

struct ActorDirectory2D : public ActorRenderBase2D
{
	std::vector<ActorRenderBase2D> Seq;
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
};
