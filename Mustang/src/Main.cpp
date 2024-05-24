#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Typedefs.h"
#include "EngineSettings.h"
#include "Logger.h"
#include "Utility.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "ShaderFactory.h"
#include "AssetLoader.h"
#include "render/Renderer.h"

#include <iostream>

void run(GLFWwindow*);

int main()
{
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow((int)EngineSettings::window_width, (int)EngineSettings::window_height, "Mustang Engine", nullptr, nullptr);
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
	ShaderFactory::Init();
	Renderer::Init();

	Renderer::AddCanvasLayer(0);

	TRY(
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	)

	double time = glfwGetTime();
	double deltaTime = 0;
	double prevTime = time;


	// posXY, texCoordRS, texSlot, RGBA
	GLfloat texSlot = 0.0f;
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, texSlot, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 1.0f, 0.0f, texSlot, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 1.0f, 1.0f, texSlot, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f, texSlot, 1.0f, 1.0f, 1.0f, 1.0f
	};
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	VertexArray va(vertices, 4, 0b1111, 0b11000101, indices, 6);

	TextureSlot* samplers = new TextureSlot[EngineSettings::max_texture_slots];
	for (TextureSlot i = 0; i < EngineSettings::max_texture_slots; i++)
		samplers[i] = i;

	ShaderHandle shader;
	auto status = loadShader("res/assets/shader.mass", shader);
	if (status != LOAD_STATUS::OK)
		ASSERT(false);

	Texture texture0("res/textures/snowman.png");
	Texture texture1("res/textures/tux.png");

	for (;;)
	{
		TRY(glClear(GL_COLOR_BUFFER_BIT));
		time = glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;

		// Render here
		Renderer::OnDraw();

		ShaderFactory::Bind(shader);
		ShaderFactory::SetUniform1iv(shader, "u_TextureSlots", EngineSettings::max_texture_slots, samplers);
		texture0.Bind(0);
		texture1.Bind(1);
		va.Bind();
		TRY(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		va.Unbind();
		texture1.Unbind(1);
		texture0.Unbind(0);
		ShaderFactory::Unbind();

		glfwSwapBuffers(window);
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}

	delete[] samplers;

	ShaderFactory::Terminate();
	Renderer::Terminate();
}
