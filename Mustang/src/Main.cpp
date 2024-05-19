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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double time = glfwGetTime();
	double deltaTime = 0;
	double prevTime = time;

	for (;;)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		time = glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;

		// Render here

		glfwSwapBuffers(window);
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}

	glfwTerminate();
	return 0;
}
