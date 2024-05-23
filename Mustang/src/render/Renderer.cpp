#include "render/Renderer.h"

#include <map>

#include "Logger.h"

std::map<ZIndex, CanvasLayer>* layers = nullptr;

static void null_map()
{
	Logger::LogErrorFatal("Renderer is not initialized. Call Renderer::Init() before application loop.");
}

void Renderer::Init()
{
	layers = new std::map<ZIndex, CanvasLayer>();
}

void Renderer::OnDraw()
{
	if (layers)
	{
		for (auto& [z, layer] : *layers)
			layer.OnDraw();
	}
	else null_map();
}

void Renderer::AddCanvasLayer(const ZIndex z)
{
	if (layers)
	{
		if (layers->find(z) == layers->end())
			layers->emplace(z, z);
		else
			Logger::LogErrorFatal(std::string("Tried to add new canvas layer to renderer zindex (") + std::to_string(z) + "), but a canvas layer under that zindex already exists!");
	}
	else null_map();
}

void Renderer::AddCanvasLayer(const CanvasLayerData data)
{
	if (layers)
	{
		if (layers->find(data.z) == layers->end())
			layers->emplace(data.z, data);
		else
			Logger::LogErrorFatal(std::string("Tried to add new canvas layer to renderer zindex (") + std::to_string(data.z) + "), but a canvas layer under that zindex already exists!");
	}
	else null_map();
}

void Renderer::RemoveCanvasLayer(const ZIndex z)
{
	if (layers)
	{
		if (layers->find(z) != layers->end())
			layers->erase(z);
		else
			Logger::LogErrorFatal(std::string("Tried to remove a canvas layer at renderer zindex (") + std::to_string(z) + "), but no canvas layer under that zindex exists!");
	}
	else null_map();
}

CanvasLayer* Renderer::GetCanvasLayer(const ZIndex z)
{
	if (layers)
	{
		auto layer = layers->find(z);
		if (layer != layers->end())
			return &layer->second;
		else
			Logger::LogErrorFatal(std::string("Tried to get a canvas layer at renderer zindex (") + std::to_string(z) + "), but no canvas layer under that zindex exists!");
	}
	else null_map();
	return nullptr;
}

void Renderer::Terminate()
{
	if (layers)
	{
		layers->clear();
		delete layers;
	}
	else null_map();
}
