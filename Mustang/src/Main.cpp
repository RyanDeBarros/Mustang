#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

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

	std::cout << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double time = glfwGetTime();
	double deltaTime = 0;
	double prevTime = time;

	float data[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.0f,  0.5f
	};
	unsigned int vb;
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (;;)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		time = glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;

		// Render here
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}

	glfwTerminate();
	return 0;
}
