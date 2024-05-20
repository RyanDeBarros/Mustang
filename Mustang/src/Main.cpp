#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Logger.h"
#include "Utility.h"
#include "Shader.h"

void run(GLFWwindow*);

int main()
{
	if (!glfwInit())
		return -1;
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

	GLfloat data[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f
	};
	unsigned int vb;
	TRY(
		glGenBuffers(1, &vb);
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data, GL_STATIC_DRAW);
	)
	TRY(
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	)

	GLuint indexes[] = {
		0, 1, 2,
		2, 3, 0
	};
	unsigned int ib;
	TRY(
		glGenBuffers(1, &ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indexes, GL_STATIC_DRAW);
	)

	Shader shader("res/shaders/test.vert", "res/shaders/test.frag");

	for (;;)
	{
		TRY(glClear(GL_COLOR_BUFFER_BIT));
		time = glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;

		// Render here
		shader.Bind();
		TRY(
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		)

		glfwSwapBuffers(window);
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}
}
