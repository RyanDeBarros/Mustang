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
	Logger::LogInfo("Welcome to Pulsar Engine! GL_VERSION:");
	TRY(Logger::LogInfo(glGetString(GL_VERSION)));
	Renderer::Init();

	Renderer::AddCanvasLayer(CanvasLayerData(0, 2048, 1024));

	double time = glfwGetTime();
	double deltaTime = 0;
	double prevTime = time;
	double totalTime = 0;

	// Load shaders
	ShaderHandle shaderStandard32;
	if (loadShader(_RendererSettings::standard_shader32_assetfile, shaderStandard32) != LOAD_STATUS::OK)
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
	//Renderable renderable;
	//if (loadRenderable("res/assets/renderable.toml", renderable) != LOAD_STATUS::OK)
		//ASSERT(false);

	// Create actors
	//ActorPrimitive2D* actor = new ActorPrimitive2D(renderable);
	RectRender* actor = new RectRender();
	Renderer::GetCanvasLayer(0)->OnAttach(actor);

	actor->SetShaderHandle(shaderStandard32);
	//actor->SetTextureHandle(textureSnowman);
	//actor->SetTextureHandle(textureTux);
	actor->SetTextureHandle(textureFlag);
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
