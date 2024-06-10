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
	//Renderable renderable;
	//if (loadRenderable("res/assets/renderable.toml", renderable) != LOAD_STATUS::OK)
		//ASSERT(false);

	// Create actors
	RectRender* actor1 = new RectRender(Transform2D{ glm::vec2(-0.6, 0.2), -1.0, glm::vec2(0.5, 0.75) });
	RectRender* actor2 = new RectRender(Transform2D{ glm::vec2(0.5, -0.3), 0.25, glm::vec2(0.6, 0.6) });
	Renderer::GetCanvasLayer(0)->OnAttach(actor1);
	//Renderer::GetCanvasLayer(0)->OnAttach(actor2);

	// testing
	actor1->SetShaderHandle(shaderStandard2);
	actor1->SetTextureHandle(textureFlag);
	actor2->SetShaderHandle(shaderStandard2);
	//actor2->SetTextureHandle(textureFlag);

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

	delete actor1;
	delete actor2;

	Renderer::Terminate();
}
