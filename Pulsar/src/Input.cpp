#include "Input.h"

#include "Macros.h"
#include "Window.h"
#include "render/Renderer.h"

#include "Logger.inl"

static void window_refresh_callback(GLFWwindow* window)
{
	Pulsar::_ExecFrame();
	PULSAR_TRY(glFinish());
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{
	// TODO
	// If _RendererSettings::resize_mode is set to SCALE_IGNORE_ASPECT_RATIO, don't add anything.
	// If it is set to SCALE_KEEP_ASPECT_RATIO, call new Renderer function that will scale objects as usual without stretching their aspect ratios.
	// If it is set to NO_SCALE_KEEP_SIZE, call new Renderer function that will not scale objects - only display more of the scene.
	PULSAR_TRY(glViewport(0, 0, width, height));
	Pulsar::_ExecFrame();
}

void Input::AssignBasicCallbacks(Window& window)
{
	//glfwSetWindowRefreshCallback(window._GetInternal(), window_refresh_callback);
	glfwSetWindowSizeCallback(window._GetInternal(), window_size_callback);

	//glfwSetCursorEnterCallback
	//glfwSetCursorPosCallback
	//glfwSetJoystickCallback
	//glfwSetKeyCallback
	//glfwSetMouseButtonCallback
	//glfwSetScrollCallback



}
