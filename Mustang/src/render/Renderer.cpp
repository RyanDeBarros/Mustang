#include "render/Renderer.h"

#include <map>

#include "Utility.h"
#include "Logger.h"
#include "ShaderFactory.h"
#include "TextureFactory.h"

std::map<CanvasIndex, CanvasLayer>* Renderer::layers = nullptr;
TextureSlot* Renderer::samplers;
std::unordered_map<BatchModel, VAO>* Renderer::rvaos;

static void null_map()
{
	Logger::LogErrorFatal("Renderer is not initialized. Call Renderer::Init() before application loop.");
}

void Renderer::Init()
{
	layers = new std::map<CanvasIndex, CanvasLayer>();
	ShaderFactory::Init();
	TextureFactory::Init();

	samplers = new TextureSlot[RenderSettings::max_texture_slots];
	for (TextureSlot i = 0; i < RenderSettings::max_texture_slots; i++)
		samplers[i] = i;

	rvaos = new std::unordered_map<BatchModel, VAO>();
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

void Renderer::AddCanvasLayer(const CanvasLayerData data)
{
	if (layers)
	{
		if (layers->find(data.ci) == layers->end())
			layers->emplace(data.ci, data);
		else
			Logger::LogErrorFatal(std::string("Tried to add new canvas layer to renderer canvas index (") + std::to_string(data.ci) + "), but a canvas layer under that canvas index already exists!");
	}
	else null_map();
}

void Renderer::RemoveCanvasLayer(const CanvasIndex ci)
{
	if (layers)
	{
		if (layers->find(ci) != layers->end())
			layers->erase(ci);
		else
			Logger::LogErrorFatal(std::string("Tried to remove a canvas layer at renderer canvas index (") + std::to_string(ci) + "), but no canvas layer under that canvas index exists!");
	}
	else null_map();
}

CanvasLayer* Renderer::GetCanvasLayer(const CanvasIndex ci)
{
	if (layers)
	{
		auto layer = layers->find(ci);
		if (layer != layers->end())
			return &layer->second;
		else
			Logger::LogErrorFatal(std::string("Tried to get a canvas layer at renderer canvas index (") + std::to_string(ci) + "), but no canvas layer under that canvas index exists!");
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
	if (samplers)
		delete[] samplers;
	TextureFactory::Terminate();
	ShaderFactory::Terminate();

	for (const auto& [model, vao] : *rvaos)
	{
		TRY(glDeleteVertexArrays(1, &vao));
	}
	if (rvaos)
		delete rvaos;
}
