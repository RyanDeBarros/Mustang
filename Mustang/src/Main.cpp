#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Logger.h"
#include "Utility.h"
#include "Shader.h"
#include "VertexArray.h"
#include "ShaderFactory.h"
#include "AssetLoader.h"

void run(GLFWwindow*);

int main()
{
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(1440, 1080, "Mustang Engine", nullptr, nullptr);
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
	TRY(Logger::LogInfo(glGetString(GL_VERSION)));

	TRY(
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	)

	double time = glfwGetTime();
	double deltaTime = 0;
	double prevTime = time;



	GLfloat vertices[] = {
		-0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f
	};
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	VertexArray va(vertices, 4, 0b11, 0b1101, indices, 6);

	ShaderHandle shader;
	auto status = loadShader("res/assets/shader.mass", shader);
	if (status != LOAD_STATUS::OK)
		ASSERT(false);

	for (;;)
	{
		TRY(glClear(GL_COLOR_BUFFER_BIT));
		time = glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;

		// Render here
		ShaderFactory::Bind(shader);
		va.Bind();
		TRY(
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		)
		va.Unbind();
		ShaderFactory::Unbind(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}

	ShaderFactory::Terminate();
}
