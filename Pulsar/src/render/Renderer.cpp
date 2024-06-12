#include "Renderer.h"

#include <GLFW/glfw3.h>
#include <map>

#include "Macros.h"
#include "Logger.h"
#include "factory/ShaderFactory.h"
#include "factory/TextureFactory.h"
#include "factory/MaterialFactory.h"
#include "factory/RectRender.h"

std::map<CanvasIndex, CanvasLayer>* Renderer::layers = nullptr;
GLFWwindow* focused_window;

static void null_map()
{
	Logger::LogErrorFatal("Renderer is not initialized. Call Renderer::Init() before application loop.");
}

void Renderer::Init()
{
	layers = new std::map<CanvasIndex, CanvasLayer>();
	ShaderFactory::Init();
	TextureFactory::Init();
	MaterialFactory::Init();
	RectRender::DefineRectRenderable();
}

void Renderer::OnDraw()
{
	if (layers)
	{
		for (auto& [z, layer] : *layers)
			layer.OnDraw();
		ForceRefresh();
	}
	else null_map();
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
	MaterialFactory::Terminate();
	TextureFactory::Terminate();
	ShaderFactory::Terminate();
}
