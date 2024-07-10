#include "ActorSequencer.h"

#include "render/CanvasLayer.h"

void ActorSequencer2D::RequestDraw(CanvasLayer* canvas_layer)
{
	canvas_layer->DrawSequencer(*this);
}
