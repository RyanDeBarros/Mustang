#include "DebugBatcher.h"

#include "render/CanvasLayer.h"

void DebugBatcher::RequestDraw(CanvasLayer* canvas_layer)
{
	for (auto& [pair, multi_polygon] : m_Slots)
	{
		canvas_layer->DrawMultiArray(multi_polygon);
	}
}
