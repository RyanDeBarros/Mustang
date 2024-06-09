#pragma once

#include <unordered_map>

#include "Typedefs.h"
#include "CanvasLayer.h"

class Renderer
{
	friend class CanvasLayer;
	static std::map<CanvasIndex, CanvasLayer>* layers;
	static std::unordered_map<BatchModel, VAO>* rvaos;
public:
	static void Init();
	static void OnDraw();
	static void AddCanvasLayer(const CanvasLayerData);
	static void RemoveCanvasLayer(const CanvasIndex);
	static CanvasLayer* GetCanvasLayer(const CanvasIndex);
	static void Terminate();
};
