#pragma once

#include "Typedefs.h"
#include "CanvasLayer.h"

class Renderer
{
public:
	static void Init();
	static void OnDraw();
	static void AddCanvasLayer(const CanvasIndex);
	static void AddCanvasLayer(const CanvasLayerData);
	static void RemoveCanvasLayer(const CanvasIndex);
	static CanvasLayer* GetCanvasLayer(const CanvasIndex);
	static void Terminate();
};
