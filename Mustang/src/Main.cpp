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

void run(GLFWwindow*);

int main()
{
	if (!_LoadRendererSettings())
		return -1;
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow((int)_RendererSettings::window_width, (int)_RendererSettings::window_height, "Mustang Engine", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
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
	Logger::LogInfo("Welcome to Mustang Engine! GL_VERSION:");
	TRY(Logger::LogInfo(glGetString(GL_VERSION)));
	Renderer::Init();

	Renderer::AddCanvasLayer(CanvasLayerData(0, 2048, 1024));

	double time = glfwGetTime();
	double deltaTime = 0;
	double prevTime = time;
	double totalTime = 0;

	// Load textures
	TextureHandle texSnowman, texTux, texFlag;
	if (loadTexture("res/assets/snowman.toml", texSnowman) != LOAD_STATUS::OK)
		ASSERT(false);
	if (loadTexture("res/assets/tux.toml", texTux) != LOAD_STATUS::OK)
		ASSERT(false);
	if (loadTexture("res/assets/flag.toml", texFlag) != LOAD_STATUS::OK)
		ASSERT(false);

	// Load renderables
	Renderable renderable;
	if (loadRenderable("res/assets/renderable.toml", renderable) != LOAD_STATUS::OK)
		ASSERT(false);

	// Create actors
	Transform2D transform;
	ActorPrimitive2D* actor = new ActorPrimitive2D(renderable, transform);
	Renderer::GetCanvasLayer(0)->OnAttach(actor);

	//actor->SetTextureHandle(texSnowman);
	//actor->SetTextureHandle(texTux);
	//actor->SetTextureHandle(texFlag);
	//actor->SetTextureHandle(0);

	for (;;)
	{
		time = glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;
		totalTime += deltaTime;
		// OnUpdate here

		// Render here
		Renderer::OnDraw();
		glfwSwapBuffers(window);
		TRY(glClear(GL_COLOR_BUFFER_BIT));
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}

	delete actor;

	Renderer::Terminate();
}
