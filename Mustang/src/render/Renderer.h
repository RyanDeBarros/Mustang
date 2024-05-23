#pragma once

#include "Typedefs.h"
#include "CanvasLayer.h"

class Renderer
{
public:
	static void Init();
	static void OnDraw();
	static void AddCanvasLayer(const ZIndex);
	static void AddCanvasLayer(const CanvasLayerData);
	static void RemoveCanvasLayer(const ZIndex);
	static CanvasLayer* GetCanvasLayer(const ZIndex);
	static void Terminate();
};
