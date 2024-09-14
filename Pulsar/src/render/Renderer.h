#pragma once

#include <unordered_map>

#include "CanvasLayer.h"

class Window;

class Renderer
{
	static std::map<CanvasIndex, CanvasLayer> layers;
public:
	static void Init();
	static void Terminate();
	static void OnDraw();
	static void FocusWindow(WindowHandle);
	static void _SetClearColor();
	static void AddCanvasLayer(const CanvasLayerData&);
	static void RemoveCanvasLayer(CanvasIndex);
	static CanvasLayer* GetCanvasLayer(CanvasIndex);
	static void ChangeCanvasLayerIndex(CanvasIndex old_index, CanvasIndex new_index);
};
