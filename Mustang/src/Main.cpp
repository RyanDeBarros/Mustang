#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Typedefs.h"
#include "EngineSettings.h"
#include "Logger.h"
#include "Utility.h"
#include "Texture.h"
#include "VertexArray.h"
#include "ShaderFactory.h"
#include "TextureFactory.h"
#include "AssetLoader.h"
#include "render/Renderer.h"

#include <iostream>

void run(GLFWwindow*);

int main()
{
	// TODO load EngineSettings via EngineSettings.ini here
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
	Renderer::Init();

	Renderer::AddCanvasLayer(0);

	TRY(
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	)

	double time = glfwGetTime();
	double deltaTime = 0;
	double prevTime = time;
	double totalTime = 0;


	// posXY, texCoordRS, texSlot, RGBA
	GLfloat texSlot = 0.0f;
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, texSlot, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 1.0f, 0.0f, texSlot, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 1.0f, 1.0f, texSlot, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f, texSlot, 1.0f, 1.0f, 1.0f, 1.0f
	};
	GLfloat* texModif[] = {
		vertices + 4, vertices + 4 + 9, vertices + 4 + 9*2, vertices + 4 + 9*3
	};
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	VertexArray va(vertices, 4, 0b1111, 0b11000101, indices, 6);

	ShaderHandle shader;
	if (loadShader("res/assets/shader.mass", shader) != LOAD_STATUS::OK)
		ASSERT(false);

	TextureHandle texH0 = TextureFactory::GetHandle("res/textures/snowman.png");
	TextureHandle texH1 = TextureFactory::GetHandle("res/textures/tux.png");

	for (;;)
	{
		TRY(glClear(GL_COLOR_BUFFER_BIT));
		time = glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;
		totalTime += deltaTime;

		// Render here
		Renderer::OnDraw();

		ShaderFactory::Bind(shader);
		ShaderFactory::SetUniform1iv(shader, "u_TextureSlots", EngineSettings::max_texture_slots, Renderer::GetSamplers());
		TextureFactory::Bind(texH0, 0);
		TextureFactory::Bind(texH1, 1);
		va.Bind();
		TRY(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		va.Unbind();
		TextureFactory::Unbind(0);
		TextureFactory::Unbind(1);
		ShaderFactory::Unbind();

		glfwSwapBuffers(window);
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}

	Renderer::Terminate();
}
