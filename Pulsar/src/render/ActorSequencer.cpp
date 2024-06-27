#include "ActorSequencer.h"

#include "CanvasLayer.h"

void ActorSequencer2D::RequestDraw(CanvasLayer* canvas_layer)
{
	canvas_layer->DrawSequencer(this);
}
