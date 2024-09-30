#include "Renderer.h"

#include <map>

#include "Macros.h"
#include "Logger.inl"
#include "render/actors/RectRender.h"
#include "platform/InputManager.h"

#ifndef PULSAR_CHECK_INITIALIZED
#if PULSAR_ASSUME_INITIALIZED
#define PULSAR_CHECK_INITIALIZED
#else
#define PULSAR_CHECK_INITIALIZED if (uninitialized)\
	Logger::LogErrorFatal("Renderer is not initialized. Call Renderer::Init() before application loop.");
#endif
#endif // PULSAR_CHECK_INITIALIZED

std::map<CanvasIndex, CanvasLayer> Renderer::layers;
WindowHandle focused_window;

ShaderRegistry* Renderer::shaders = nullptr;
TextureRegistry* Renderer::textures = nullptr;
TileRegistry* Renderer::tiles = nullptr;
UniformLexiconRegistry* Renderer::uniform_lexicons = nullptr;

#if !PULSAR_ASSUME_INITIALIZED
bool uninitialized = true;
#endif

void Renderer::Init()
{
#if !PULSAR_ASSUME_INITIALIZED
	uninitialized = false;
#endif
	if (!shaders)
		shaders = new ShaderRegistry();
	shaders->DefineStandardShader();
	if (!textures)
		textures = new TextureRegistry();
	if (!tiles)
		tiles = new TileRegistry();
	if (!uniform_lexicons)
		uniform_lexicons = new UniformLexiconRegistry();
	InputManager::Instance(); // TODO put somewhere else?
	RectRender::DefineRectRenderable();
	PULSAR_TRY(glEnable(GL_PROGRAM_POINT_SIZE));
	_SetClearColor();
}

void Renderer::Terminate()
{
	PULSAR_CHECK_INITIALIZED
#if !PULSAR_ASSUME_INITIALIZED
	uninitialized = true;
#endif
	layers.clear();
	RectRender::DestroyRectRenderable();
	if (shaders)
	{
		delete shaders;
		shaders = nullptr;
	}
	if (textures)
	{
		delete textures;
		textures = nullptr;
	}
	if (tiles)
	{
		delete tiles;
		tiles = nullptr;
	}
	if (uniform_lexicons)
	{
		delete uniform_lexicons;
		uniform_lexicons = nullptr;
	}
}

void Renderer::OnDraw()
{
	PULSAR_CHECK_INITIALIZED
	for (auto& [z, layer] : layers)
		layer.OnDraw();
	WindowManager::GetWindow(focused_window)->_ForceRefresh();
}

void Renderer::FocusWindow(WindowHandle window)
{
	focused_window = window;
}

void Renderer::_SetClearColor()
{
	PULSAR_TRY(glClearColor(PulsarSettings::gl_clear_color()[0], PulsarSettings::gl_clear_color()[1],
		PulsarSettings::gl_clear_color()[2], PulsarSettings::gl_clear_color()[3]));
}

void Renderer::AddCanvasLayer(const CanvasLayerData& data)
{
	PULSAR_CHECK_INITIALIZED
	if (layers.find(data.ci) == layers.end())
		layers.emplace(data.ci, data);
	else
		Logger::LogErrorFatal(std::string("Tried to add new canvas layer to renderer canvas index (") + std::to_string(data.ci)
			+ "), but a canvas layer under that canvas index already exists!");
}

void Renderer::RemoveCanvasLayer(CanvasIndex ci)
{
	PULSAR_CHECK_INITIALIZED
	auto layer_it = layers.find(ci);
	if (layer_it != layers.end())
		layers.erase(layer_it);
	else
		Logger::LogErrorFatal(std::string("Tried to remove a canvas layer at renderer canvas index (") + std::to_string(ci)
			+ "), but no canvas layer under that canvas index exists!");
}

CanvasLayer* Renderer::GetCanvasLayer(CanvasIndex ci)
{
	PULSAR_CHECK_INITIALIZED
	auto layer = layers.find(ci);
	if (layer != layers.end())
		return &layer->second;
	else
	{
		Logger::LogErrorFatal(std::string("Tried to get a canvas layer at renderer canvas index (") + std::to_string(ci)
			+ "), but no canvas layer under that canvas index exists!");
		return nullptr;
	}
}

void Renderer::ChangeCanvasLayerIndex(CanvasIndex old_index, CanvasIndex new_index)
{
	auto layer_it = layers.find(old_index);
	if (layer_it == layers.end())
	{
		Logger::LogErrorFatal(std::string("Tried to change canvas layer index from " + std::to_string(old_index)
			+ " to " + std::to_string(new_index) + ", but the old index does not exist."));
	}
	else if (layers.find(new_index) != layers.end())
	{
		Logger::LogErrorFatal(std::string("Tried to change canvas layer index from " + std::to_string(old_index)
			+ " to " + std::to_string(new_index) + ", but the new index already exists."));
	}
	else
	{
		auto pair = layers.extract(layer_it);
		pair.mapped().m_Data.ci = new_index;
		pair.key() = new_index;
		layers.insert(std::move(pair));
	}
}
