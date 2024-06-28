#pragma once

#include <unordered_map>

#include "Typedefs.h"
#include "CanvasLayer.h"

class Renderer
{
	static std::map<CanvasIndex, CanvasLayer> layers;
	static void ForceRefresh();
public:
	static void Init();
	static void FocusWindow(struct GLFWwindow*);
	static void OnDraw();
	static void AddCanvasLayer(const CanvasLayerData);
	static void RemoveCanvasLayer(const CanvasIndex);
	static CanvasLayer* GetCanvasLayer(const CanvasIndex);
	static void Terminate();
};
