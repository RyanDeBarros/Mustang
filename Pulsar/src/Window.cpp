#include "Window.h"

#include "Macros.h"
#include "Input.h"
#include "render/Renderer.h"

Window::Window(unsigned int width, unsigned int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
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

// TODO for all these _Callback()s, call all connected input event handlers.


static void default_window_refresh()
{
	Pulsar::_ExecFrame();
	PULSAR_TRY(glFinish());
}

void Window::_CallbackWindowRefresh()
{
	default_window_refresh();
}

static void default_window_resize(int width, int height)
{
	// TODO
	// If _RendererSettings::resize_mode is set to SCALE_IGNORE_ASPECT_RATIO, don't add anything.
	// If it is set to SCALE_KEEP_ASPECT_RATIO, call new Renderer function that will scale objects as usual without stretching their aspect ratios.
	// If it is set to NO_SCALE_KEEP_SIZE, call new Renderer function that will not scale objects - only display more of the scene.
	PULSAR_TRY(glViewport(0, 0, width, height));
	Pulsar::_ExecFrame();
}

void Window::_CallbackWindowResize(int width, int height)
{
	default_window_resize(width, height);
}

void Window::_CallbackWindowClose()
{
}

void Window::_CallbackWindowContentScale(float sx, float sy)
{
}

void Window::_CallbackWindowFocus(bool focused)
{
}

void Window::_CallbackWindowIconify(bool iconified)
{
}

void Window::_CallbackWindowMaximize(bool maximized)
{
}

void Window::_CallbackWindowPos(int x, int y)
{
}

void Window::_CallbackCursorEnter(bool entered)
{
}

void Window::_CallbackCursorPos(double x, double y)
{
}

void Window::_CallbackPathDrop(int path_count, const char** paths)
{
}

void Window::_CallbackKey(int key, int scancode, int action, int mods)
{
}

void Window::_CallbackMouseButton(int button, int action, int mods)
{
}

void Window::_CallbackScroll(double xoff, double yoff)
{
}
