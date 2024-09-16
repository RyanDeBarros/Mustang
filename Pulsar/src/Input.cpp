#include "Input.h"

#include "Macros.h"
#include "Window.h"
#include "render/Renderer.h"

#include "Logger.inl"

static void window_close_callback(GLFWwindow* window)
{
	WindowManager::_GetWindow(window)->_CallbackWindowClose();
}

static void window_content_scale_callback(GLFWwindow* window, float sx, float sy)
{
	WindowManager::_GetWindow(window)->_CallbackWindowContentScale(sx, sy);
}

static void window_focus_callback(GLFWwindow* window, int focused)
{
	WindowManager::_GetWindow(window)->_CallbackWindowFocus(static_cast<bool>(focused));
}

static void window_iconify_callback(GLFWwindow* window, int iconified)
{
	WindowManager::_GetWindow(window)->_CallbackWindowIconify(static_cast<bool>(iconified));
}

static void window_maximize_callback(GLFWwindow* window, int maximized)
{
	WindowManager::_GetWindow(window)->_CallbackWindowMaximize(static_cast<bool>(maximized));
}

static void window_pos_callback(GLFWwindow* window, int x, int y)
{
	WindowManager::_GetWindow(window)->_CallbackWindowPos(x, y);
}

static void window_refresh_callback(GLFWwindow* window)
{
	WindowManager::_GetWindow(window)->_CallbackWindowRefresh();
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{
	WindowManager::_GetWindow(window)->_CallbackWindowResize(width, height);
}

static void cursor_enter_callback(GLFWwindow* window, int entered)
{
	WindowManager::_GetWindow(window)->_CallbackCursorEnter(static_cast<bool>(entered));
}

static void cursor_pos_callback(GLFWwindow* window, double x, double y)
{
	WindowManager::_GetWindow(window)->_CallbackCursorPos(x, y);
}

static void path_drop_callback(GLFWwindow* window, int path_count, const char** paths)
{
	WindowManager::_GetWindow(window)->_CallbackPathDrop(path_count, paths);
}

static void joystick_callback(int jid, int event)
{
	// TODO
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	WindowManager::_GetWindow(window)->_CallbackKey(key, scancode, action, mods);
}

static void monitor_callback(GLFWmonitor* monitor, int event)
{
	// TODO
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	WindowManager::_GetWindow(window)->_CallbackMouseButton(button, action, mods);
}

static void scroll_callback(GLFWwindow* window, double xoff, double yoff)
{
	WindowManager::_GetWindow(window)->_CallbackScroll(xoff, yoff);
}

void Input::AssignBasicCallbacks(Window& window)
{
	glfwSetWindowCloseCallback(window._GetInternal(), window_close_callback);
	glfwSetWindowContentScaleCallback(window._GetInternal(), window_content_scale_callback);
	glfwSetWindowFocusCallback(window._GetInternal(), window_focus_callback);
	glfwSetWindowIconifyCallback(window._GetInternal(), window_iconify_callback);
	glfwSetWindowMaximizeCallback(window._GetInternal(), window_maximize_callback);
	glfwSetWindowPosCallback(window._GetInternal(), window_pos_callback);
	//glfwSetWindowRefreshCallback(window._GetInternal(), window_refresh_callback);
	glfwSetWindowSizeCallback(window._GetInternal(), window_size_callback);
	
	glfwSetCursorEnterCallback(window._GetInternal(), cursor_enter_callback);
	glfwSetCursorPosCallback(window._GetInternal(), cursor_pos_callback);
	glfwSetDropCallback(window._GetInternal(), path_drop_callback);
	//glfwSetFramebufferSizeCallback;
	glfwSetJoystickCallback(joystick_callback);
	glfwSetKeyCallback(window._GetInternal(), key_callback);
	glfwSetMonitorCallback(monitor_callback);
	glfwSetMouseButtonCallback(window._GetInternal(), mouse_button_callback);
	glfwSetScrollCallback(window._GetInternal(), scroll_callback);
}

InputManager::InputManager()
{

}

InputManager::~InputManager()
{
}
