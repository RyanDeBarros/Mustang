#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Typedefs.h"
#include "RendererSettings.h"
#include "Logger.h"
#include "Macros.h"
#include "AssetLoader.h"
#include "render/Renderer.h"
#include "render/ActorPrimitive.h"
#include "factory/RectRender.h"

void run(GLFWwindow*);

void window_refresh_callback(GLFWwindow* window)
{
	Renderer::OnDraw(window);
	TRY(glFinish()); // important, this waits until rendering result is actually visible, thus making resizing less ugly
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// TODO
	// If _RendererSettings::resize_mode is set to SCALE_IGNORE_ASPECT_RATIO, don't add anything.
	// If it is set to SCALE_KEEP_ASPECT_RATIO, call new Renderer function that will scale objects as usual without stretching their aspect ratios.
	// If it is set to NO_SCALE_KEEP_SIZE, call new Renderer function that will not scale objects - only display more of the scene.
	TRY(glViewport(0, 0, width, height));
}

int main()
{
	if (!_LoadRendererSettings())
		return -1;
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	GLFWwindow* window = glfwCreateWindow((int)_RendererSettings::initial_window_width, (int)_RendererSettings::initial_window_height, "Mustang Engine", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetWindowRefreshCallback(window, window_refresh_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		return -1;
	}
	run(window);
	glfwTerminate();
	return 0;
}

void run(GLFWwindow* window)
{
	Logger::LogInfo("Welcome to Pulsar Engine! GL_VERSION:");
	TRY(Logger::LogInfo(glGetString(GL_VERSION)));
	Renderer::Init();

	double time = glfwGetTime();
	double deltaTime = 0;
	double prevTime = time;
	double totalTime = 0;

	// Load shaders
	ShaderHandle shaderStandard32;
	if (loadShader(_RendererSettings::standard_shader32_assetfile, shaderStandard32) != LOAD_STATUS::OK)
		ASSERT(false);
	ShaderHandle shaderStandard2;
	if (loadShader("res/shaders/StandardShader2.toml", shaderStandard2) != LOAD_STATUS::OK)
		ASSERT(false);

	// Load textures
	TextureHandle textureSnowman, textureTux, textureFlag;
	if (loadTexture("res/assets/snowman.toml", textureSnowman) != LOAD_STATUS::OK)
		ASSERT(false);
	if (loadTexture("res/assets/tux.toml", textureTux) != LOAD_STATUS::OK)
		ASSERT(false);
	if (loadTexture("res/assets/flag.toml", textureFlag) != LOAD_STATUS::OK)
		ASSERT(false);

	// Load renderables
	Renderable renderable;
	if (loadRenderable("res/assets/renderable.toml", renderable) != LOAD_STATUS::OK)
		ASSERT(false);

	Renderer::AddCanvasLayer(CanvasLayerData(0, _RendererSettings::standard_vertex_pool_size, _RendererSettings::standard_index_pool_size));

	// Create actors

	RectRender* actor1 = new RectRender(Transform2D{ glm::vec2(-600, 400), -1.0, glm::vec2(0.8, 1.2) }, textureFlag, shaderStandard2);
	RectRender* actor2 = new RectRender(Transform2D{ glm::vec2(400, -200), 0.25, glm::vec2(1.0, 1.0) }, textureSnowman, shaderStandard2);
	RectRender* actor3 = new RectRender(Transform2D{ glm::vec2(0.0, 0.0), 0.0, glm::vec2(1.0, 1.0) }, textureTux, shaderStandard2);

	Renderer::GetCanvasLayer(0)->OnAttach(actor1);
	Renderer::GetCanvasLayer(0)->OnAttach(actor2);

	Renderer::GetCanvasLayer(0)->OnSetZIndex(actor1, 1);

	Renderer::GetCanvasLayer(0)->OnAttach(actor3);

	Renderer::AddCanvasLayer(CanvasLayerData(-1, _RendererSettings::standard_vertex_pool_size, _RendererSettings::standard_index_pool_size));

	ActorPrimitive2D* actor4 = new ActorPrimitive2D(renderable, Transform2D{ glm::vec2(-400.0, 0.0), 0.0, glm::vec2(800, 800) });
	Renderer::GetCanvasLayer(-1)->OnAttach(actor4);

	actor1->SetScale(20.0, 20.0);
	//Renderer::GetCanvasLayer(0)->OnSetZIndex(actor3, -1);

	actor3->SetPivot(0, 0);
	//actor3->SetPosition(0, 0);
	actor3->SetPosition(-_RendererSettings::initial_window_width * 0.5, -_RendererSettings::initial_window_height * 0.5);
	actor3->SetScale(0.3, 0.3);
	//actor2->SetRotation(0);
	//actor2->SetPosition(0, 0);

	for (;;)
	{
		time = glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;
		totalTime += deltaTime;
		// OnUpdate here

		// Render here
		Renderer::OnDraw(window);
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}

	delete actor1, actor2, actor3, actor4;

	Renderer::Terminate();
}
