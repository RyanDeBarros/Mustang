#include "Window.h"

#include "Macros.h"
#include "InputManager.h"
#include "render/Renderer.h"

static void default_window_refresh()
{
	Pulsar::_ExecFrame();
	PULSAR_TRY(glFinish());
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

Window::Window(unsigned int width, unsigned int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	window = glfwCreateWindow(width, height, title, monitor, share);
	if (!window)
		throw WindowException("glfwCreateWindow() failed in Window constructor.");
	Focus();
	if (_RendererSettings::vsync_on)
		glfwSwapInterval(1);
	InputManager::Instance().AssignWindowCallbacks(*this);
	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(window);
		window = nullptr;
		throw WindowException("glewInit() failed in Window constructor.");
	}
}

void Window::_Register(WindowHandle handle)
{
	InputManager::Instance().DispatchWindowRefresh().Connect({ handle },
		make_functor_ptr([](const InputEvent::WindowRefresh& wr) { default_window_refresh(); }));
	InputManager::Instance().DispatchWindowResize().Connect({ handle },
		make_functor_ptr([](const InputEvent::WindowResize& wr) { default_window_resize(wr.width, wr.height); }));
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
