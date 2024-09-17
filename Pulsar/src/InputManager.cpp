#include "InputManager.h"

#include "Window.h"

static void window_close_callback(GLFWwindow* window)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
		InputManager::Instance().DispatchWindowClose().Emit({ handle });
}

static void window_content_scale_callback(GLFWwindow* window, float sx, float sy)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
		InputManager::Instance().DispatchWindowContentScale().Emit({ handle, sx, sy });
}

static void window_focus_callback(GLFWwindow* window, int focused)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
		InputManager::Instance().DispatchWindowFocus().Emit({ handle, static_cast<bool>(focused) });
}

static void window_iconify_callback(GLFWwindow* window, int iconified)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
		InputManager::Instance().DispatchWindowIconify().Emit({ handle, static_cast<bool>(iconified) });
}

static void window_maximize_callback(GLFWwindow* window, int maximized)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
		InputManager::Instance().DispatchWindowMaximize().Emit({ handle, static_cast<bool>(maximized) });
}

static void window_pos_callback(GLFWwindow* window, int x, int y)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
		InputManager::Instance().DispatchWindowPos().Emit({ handle, x, y });
}

static void window_refresh_callback(GLFWwindow* window)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
		InputManager::Instance().DispatchWindowRefresh().Emit({ handle });
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
		InputManager::Instance().DispatchWindowResize().Emit({ handle, width, height });
}

static void cursor_enter_callback(GLFWwindow* window, int entered)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
		InputManager::Instance().DispatchCursorEnter().Emit({ handle, static_cast<bool>(entered) });
}

static void cursor_pos_callback(GLFWwindow* window, double x, double y)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
		InputManager::Instance().DispatchCursorPos().Emit({ handle, static_cast<float>(x), static_cast<float>(y) });
}

static void path_drop_callback(GLFWwindow* window, int path_count, const char** paths)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
	{
		std::vector<std::string> v_paths;
		for (int i = 0; i < path_count; ++i)
			v_paths.push_back(std::string(paths[i]));
		InputManager::Instance().DispatchPathDrop().Emit({ handle, std::move(v_paths) });
	}
}

static void joystick_callback(int jid, int event)
{
	InputManager::Instance().DispatchControllerConnect().Emit({ jid, static_cast<bool>(event) });
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
		InputManager::Instance().DispatchKey().Emit({ handle, static_cast<Input::Key>(key), static_cast<Input::Action>(action), scancode, mods });
}

static void monitor_callback(GLFWmonitor* monitor, int event)
{
	InputManager::Instance().DispatchMonitorConnect().Emit({ monitor, static_cast<bool>(event) });
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
		InputManager::Instance().DispatchMouseButton().Emit({ handle, static_cast<Input::MouseButton>(button), static_cast<Input::Action>(action), mods });
}

static void scroll_callback(GLFWwindow* window, double xoff, double yoff)
{
	WindowHandle handle;
	if (WindowManager::_GetHandle(window, handle))
		InputManager::Instance().DispatchScroll().Emit({ handle, xoff, yoff });
}

InputManager::InputManager()
	: h_WindowClose(InputData::WindowClose::Separator),
	h_WindowContentScale(InputData::WindowContentScale::Separator),
	h_WindowFocus(InputData::WindowFocus::Separator),
	h_WindowIconify(InputData::WindowIconify::Separator),
	h_WindowMaximize(InputData::WindowMaximize::Separator),
	h_WindowPos(InputData::WindowPos::Separator),
	h_WindowRefresh(InputData::WindowRefresh::Separator),
	h_WindowResize(InputData::WindowResize::Separator),
	h_CursorEnter(InputData::CursorEnter::Separator),
	h_CursorPos(InputData::CursorPos::Separator),
	h_PathDrop(InputData::PathDrop::Separator),
	h_ControllerConnect(InputData::ControllerConnect::Separator),
	h_Key(InputData::Key::Separator),
	h_MonitorConnect(InputData::MonitorConnect::Separator),
	h_MouseButton(InputData::MouseButton::Separator),
	h_Scroll(InputData::Scroll::Separator)
{
	glfwSetJoystickCallback(joystick_callback);
	glfwSetMonitorCallback(monitor_callback);
}

void InputManager::AssignWindowCallbacks(Window& window)
{
	glfwSetWindowCloseCallback(window._GetInternal(), window_close_callback);
	glfwSetWindowContentScaleCallback(window._GetInternal(), window_content_scale_callback);
	glfwSetWindowFocusCallback(window._GetInternal(), window_focus_callback);
	glfwSetWindowIconifyCallback(window._GetInternal(), window_iconify_callback);
	glfwSetWindowMaximizeCallback(window._GetInternal(), window_maximize_callback);
	glfwSetWindowPosCallback(window._GetInternal(), window_pos_callback);
	// NOTE window refresh and framebuffer size callbacks are not enabled in InputManager.
	//glfwSetWindowRefreshCallback(window._GetInternal(), window_refresh_callback);
	glfwSetWindowSizeCallback(window._GetInternal(), window_size_callback);

	glfwSetCursorEnterCallback(window._GetInternal(), cursor_enter_callback);
	glfwSetCursorPosCallback(window._GetInternal(), cursor_pos_callback);
	glfwSetDropCallback(window._GetInternal(), path_drop_callback);
	//glfwSetFramebufferSizeCallback(window._GetInternal(), window_framebuffer_size_callback);
	glfwSetKeyCallback(window._GetInternal(), key_callback);
	glfwSetMouseButtonCallback(window._GetInternal(), mouse_button_callback);
	glfwSetScrollCallback(window._GetInternal(), scroll_callback);
}
