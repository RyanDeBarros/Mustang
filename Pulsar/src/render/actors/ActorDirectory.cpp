#include "ActorDirectory.h"

void ActorDirectory2D::RequestDraw(CanvasLayer* canvas_layer)
{
	for (auto iter = Seq.begin(); iter != Seq.end(); iter++)
		iter->RequestDraw(canvas_layer);
}
