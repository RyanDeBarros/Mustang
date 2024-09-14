#include "Window.h"

#include "Macros.h"
#include "Input.h"
#include "render/Renderer.h"

Window::Window(unsigned int width, unsigned int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
	: width(width), height(height)
{
	window = glfwCreateWindow(width, height, title, monitor, share);
	if (!window)
		throw WindowException("glfwCreateWindow() failed in Window constructor.");
	Focus();
	if (_RendererSettings::vsync_on)
		glfwSwapInterval(1);
	Input::AssignBasicCallbacks(*this);
	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(window);
		window = nullptr;
		throw WindowException("glewInit() failed in Window constructor.");
	}
}

void Window::Focus() const
{
	glfwMakeContextCurrent(window);
}

void Window::_ForceRefresh() const
{
	glfwSwapBuffers(window);
	PULSAR_TRY(glClear(GL_COLOR_BUFFER_BIT));
}
