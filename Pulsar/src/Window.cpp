#include "Window.h"

#include "Macros.h"
#include "render/Renderer.h"

static void window_refresh_callback(GLFWwindow* window)
{
	Renderer::OnDraw();
	PULSAR_TRY(glFinish()); // important, this waits until rendering result is actually visible, thus making resizing less ugly
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// TODO
	// If _RendererSettings::resize_mode is set to SCALE_IGNORE_ASPECT_RATIO, don't add anything.
	// If it is set to SCALE_KEEP_ASPECT_RATIO, call new Renderer function that will scale objects as usual without stretching their aspect ratios.
	// If it is set to NO_SCALE_KEEP_SIZE, call new Renderer function that will not scale objects - only display more of the scene.
	// Also update frame in callback. Currently, animation is paused when resizing.
	PULSAR_TRY(glViewport(0, 0, width, height));
	Renderer::OnDraw();
}

Window::Window(unsigned int width, unsigned int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
	: width(width), height(height)
{
	window = glfwCreateWindow(width, height, title, monitor, share);
	if (!window)
		throw WindowException("glfwCreateWindow() failed in Window constructor.");
	Focus();
	if (_RendererSettings::vsync_on)
		glfwSwapInterval(1);
	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(window);
		window = nullptr;
		throw WindowException("glewInit() failed in Window constructor.");
	}
	// TODO move these callbacks to Input
	glfwSetWindowRefreshCallback(window, window_refresh_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void Window::Focus() const
{
	glfwMakeContextCurrent(window);
}

void Window::ForceRefresh() const
{
	glfwSwapBuffers(window);
	PULSAR_TRY(glClear(GL_COLOR_BUFFER_BIT));
}
