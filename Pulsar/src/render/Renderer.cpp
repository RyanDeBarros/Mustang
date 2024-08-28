#include "Renderer.h"

#include <GLFW/glfw3.h>
#include <map>

#include "Macros.h"
#include "Logger.inl"
#include "factory/ShaderRegistry.h"
#include "factory/TextureRegistry.h"
#include "factory/UniformLexiconRegistry.h"
#include "factory/TileRegistry.h"
#include "render/actors/RectRender.h"

#ifndef PULSAR_CHECK_INITIALIZED
#if PULSAR_ASSUME_INITIALIZED
#define PULSAR_CHECK_INITIALIZED
#else
#define PULSAR_CHECK_INITIALIZED if (uninitialized)\
	Logger::LogErrorFatal("Renderer is not initialized. Call Renderer::Init() before application loop.");
#endif
#endif // PULSAR_CHECK_INITIALIZED

std::map<CanvasIndex, CanvasLayer> Renderer::layers;
GLFWwindow* focused_window;

#if !PULSAR_ASSUME_INITIALIZED
bool uninitialized = true;
#endif

void Renderer::Init()
{
#if !PULSAR_ASSUME_INITIALIZED
	uninitialized = false;
#endif
	ShaderRegistry::Init();
	TextureRegistry::Init();
	UniformLexiconRegistry::Init();
	TileRegistry::Init();
	RectRender::DefineRectRenderable();
	PULSAR_TRY(glEnable(GL_PROGRAM_POINT_SIZE));
}

void Renderer::Terminate()
{
	PULSAR_CHECK_INITIALIZED
#if !PULSAR_ASSUME_INITIALIZED
	uninitialized = true;
#endif
	layers.clear();
	TileRegistry::Terminate();
	UniformLexiconRegistry::Terminate();
	TextureRegistry::Terminate();
	ShaderRegistry::Terminate();
}

void Renderer::OnDraw()
{
	PULSAR_CHECK_INITIALIZED
	for (auto& [z, layer] : layers)
		layer.OnDraw();
	_ForceRefresh();
}

void Renderer::FocusWindow(GLFWwindow* window)
{
	focused_window = window;
}

void Renderer::_ForceRefresh()
{
	glfwSwapBuffers(focused_window);
	PULSAR_TRY(glClear(GL_COLOR_BUFFER_BIT));
	// TODO setting to not clear color, in order to save performance. most scenes will have a background anyways.
	PULSAR_TRY(glClearColor(_RendererSettings::gl_clear_color[0], _RendererSettings::gl_clear_color[1], _RendererSettings::gl_clear_color[2], _RendererSettings::gl_clear_color[3]));
}

void Renderer::AddCanvasLayer(const CanvasLayerData& data)
{
	PULSAR_CHECK_INITIALIZED
	if (layers.find(data.ci) == layers.end())
		layers.emplace(data.ci, data);
	else
		Logger::LogErrorFatal(std::string("Tried to add new canvas layer to renderer canvas index (") + std::to_string(data.ci) + "), but a canvas layer under that canvas index already exists!");
}

void Renderer::RemoveCanvasLayer(CanvasIndex ci)
{
	PULSAR_CHECK_INITIALIZED
	auto layer_it = layers.find(ci);
	if (layer_it != layers.end())
		layers.erase(layer_it);
	else
		Logger::LogErrorFatal(std::string("Tried to remove a canvas layer at renderer canvas index (") + std::to_string(ci) + "), but no canvas layer under that canvas index exists!");
}

CanvasLayer* Renderer::GetCanvasLayer(CanvasIndex ci)
{
	PULSAR_CHECK_INITIALIZED
	auto layer = layers.find(ci);
	if (layer != layers.end())
		return &layer->second;
	else
	{
		Logger::LogErrorFatal(std::string("Tried to get a canvas layer at renderer canvas index (") + std::to_string(ci) + "), but no canvas layer under that canvas index exists!");
		return nullptr;
	}
}

void Renderer::ChangeCanvasLayerIndex(CanvasIndex old_index, CanvasIndex new_index)
{
	auto layer_it = layers.find(old_index);
	if (layer_it == layers.end())
	{
		Logger::LogErrorFatal(std::string("Tried to change canvas layer index from " + std::to_string(old_index) + " to " + std::to_string(new_index) + ", but the old index does not exist."));
	}
	else if (layers.find(new_index) != layers.end())
	{
		Logger::LogErrorFatal(std::string("Tried to change canvas layer index from " + std::to_string(old_index) + " to " + std::to_string(new_index) + ", but the new index already exists."));
	}
	else
	{
		auto pair = layers.extract(layer_it);
		pair.mapped().m_Data.ci = new_index;
		pair.key() = new_index;
		layers.insert(std::move(pair));
	}
}
