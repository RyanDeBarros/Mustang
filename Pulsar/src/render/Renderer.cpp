#include "Renderer.h"

#include <GLFW/glfw3.h>
#include <map>

#include "Macros.h"
#include "Logger.h"
#include "factory/ShaderFactory.h"
#include "factory/TextureFactory.h"
#include "factory/UniformLexiconFactory.h"
#include "factory/TileFactory.h"
#include "render/RectRender.h"

std::map<CanvasIndex, CanvasLayer> Renderer::layers;
GLFWwindow* focused_window;

bool uninitialized = true;

#ifdef PULSAR_ASSUME_INITIALIZED
#define CHECK_INITIALIZED check_initialized();
#else
#define CHECK_INITIALIZED
#endif

static void check_initialized()
{
	if (uninitialized)
		Logger::LogErrorFatal("Renderer is not initialized. Call Renderer::Init() before application loop.");
}

void Renderer::Init()
{
	uninitialized = false;
	ShaderFactory::Init();
	TextureFactory::Init();
	UniformLexiconFactory::Init();
	TileFactory::Init();
	RectRender::DefineRectRenderable();
}

void Renderer::Terminate()
{
	CHECK_INITIALIZED
	uninitialized = true;
	layers.clear();
	TileFactory::Terminate();
	UniformLexiconFactory::Terminate();
	TextureFactory::Terminate();
	ShaderFactory::Terminate();
}

void Renderer::OnDraw()
{
	CHECK_INITIALIZED
	for (auto& [z, layer] : layers)
		layer.OnDraw();
	ForceRefresh();
}

void Renderer::FocusWindow(GLFWwindow* window)
{
	focused_window = window;
}

void Renderer::ForceRefresh()
{
	glfwSwapBuffers(focused_window);
	TRY(glClear(GL_COLOR_BUFFER_BIT));
	TRY(glClearColor(_RendererSettings::gl_clear_color[0], _RendererSettings::gl_clear_color[1], _RendererSettings::gl_clear_color[2], _RendererSettings::gl_clear_color[3]));
}

void Renderer::AddCanvasLayer(const CanvasLayerData data)
{
	CHECK_INITIALIZED
	if (layers.find(data.ci) == layers.end())
		layers.emplace(data.ci, data);
	else
		Logger::LogErrorFatal(std::string("Tried to add new canvas layer to renderer canvas index (") + std::to_string(data.ci) + "), but a canvas layer under that canvas index already exists!");
}

void Renderer::RemoveCanvasLayer(const CanvasIndex ci)
{
	CHECK_INITIALIZED
	if (layers.find(ci) != layers.end())
		layers.erase(ci);
	else
		Logger::LogErrorFatal(std::string("Tried to remove a canvas layer at renderer canvas index (") + std::to_string(ci) + "), but no canvas layer under that canvas index exists!");
}

CanvasLayer* Renderer::GetCanvasLayer(const CanvasIndex ci)
{
	CHECK_INITIALIZED
	auto layer = layers.find(ci);
	if (layer != layers.end())
		return &layer->second;
	else
	{
		Logger::LogErrorFatal(std::string("Tried to get a canvas layer at renderer canvas index (") + std::to_string(ci) + "), but no canvas layer under that canvas index exists!");
		return nullptr;
	}
}
