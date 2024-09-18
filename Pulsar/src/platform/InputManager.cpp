#include "InputManager.h"

#include "Window.h"
#include "PulsarSettings.h"
#include "Logger.inl"
#include "IO.h"

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
	InputManager::Instance().DispatchControllerConnect().Emit({ static_cast<Input::ControllerID>(jid), static_cast<bool>(event) });
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	WindowHandle handle;
	// TODO also have regular event dispatcher that emits generic InputSource so that key can then be checked in handler. Similar in other callbacks
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
	: h_WindowClose(InputSource::WindowClose::Separator),
	h_WindowContentScale(InputSource::WindowContentScale::Separator),
	h_WindowFocus(InputSource::WindowFocus::Separator),
	h_WindowIconify(InputSource::WindowIconify::Separator),
	h_WindowMaximize(InputSource::WindowMaximize::Separator),
	h_WindowPos(InputSource::WindowPos::Separator),
	h_WindowRefresh(InputSource::WindowRefresh::Separator),
	h_WindowResize(InputSource::WindowResize::Separator),
	h_CursorEnter(InputSource::CursorEnter::Separator),
	h_CursorPos(InputSource::CursorPos::Separator),
	h_PathDrop(InputSource::PathDrop::Separator),
	h_ControllerConnect(InputSource::ControllerConnect::Separator),
	h_Key(InputSource::Key::Separator),
	h_MonitorConnect(InputSource::MonitorConnect::Separator),
	h_MouseButton(InputSource::MouseButton::Separator),
	h_Scroll(InputSource::Scroll::Separator)
{
	glfwSetJoystickCallback(joystick_callback);
	glfwSetMonitorCallback(monitor_callback);
	std::string mapping;
	if (IO::read_file(_PulsarSettings::sdl_gamecontrollerdb.c_str(), mapping))
		LoadGamepadMapping(mapping.c_str());
	else
		Logger::LogErrorFatal("Cannot load SDL_GameControllerDB.");
}

void InputManager::AssignWindowCallbacks(Window& window)
{
	// TODO do these: These callbacks should be used for text input.
	glfwSetCharCallback;
	glfwSetCharModsCallback;

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

Input::Action InputManager::GetKey(Input::Key key, WindowHandle window)
{
	return static_cast<Input::Action>(glfwGetKey(WindowManager::GetWindow(window)->_GetInternal(), static_cast<int>(key)));
}

Input::Action InputManager::GetMouseButton(Input::MouseButton mb, WindowHandle window)
{
	return static_cast<Input::Action>(glfwGetMouseButton(WindowManager::GetWindow(window)->_GetInternal(), static_cast<int>(mb)));
}

Input::Action InputManager::GetKeySafe(Input::Key key, WindowHandle window)
{
	if (auto win = WindowManager::GetWindow(window))
		return static_cast<Input::Action>(glfwGetKey(win->_GetInternal(), static_cast<int>(key)));
	else
		throw WindowException("Window handle (" + std::to_string(window) + ") does not exist.");
}

Input::Action InputManager::GetMouseButtonSafe(Input::MouseButton mb, WindowHandle window)
{
	if (auto win = WindowManager::GetWindow(window))
		return static_cast<Input::Action>(glfwGetMouseButton(win->_GetInternal(), static_cast<int>(mb)));
	else
		throw WindowException("Window handle (" + std::to_string(window) + ") does not exist.");
}

Input::Action InputManager::GetControllerButton(Input::Gamepad::Button button, Input::ControllerID jid)
{
	int count;
	return static_cast<Input::Action>(glfwGetJoystickButtons(static_cast<int>(jid), &count)[static_cast<int>(button)]);
}

float InputManager::GetControllerAxis(Input::Gamepad::Axis axis, Input::ControllerID jid)
{
	int count;
	return glfwGetJoystickAxes(static_cast<int>(jid), &count)[static_cast<int>(axis)];
}

Input::Action InputManager::GetControllerButtonSafe(Input::Gamepad::Button button, Input::ControllerID jid)
{
	int count;
	unsigned char const* buttons = glfwGetJoystickButtons(static_cast<int>(jid), &count);
	if (count > static_cast<int>(button))
		return static_cast<Input::Action>(buttons[static_cast<int>(button)]);
	else
		throw InputException("Controller (" + std::to_string(static_cast<int>(jid)) + ") is not connected, the button ("
			+ std::to_string(static_cast<int>(button)) + ") is not supported, or a GLFW error occurred.");
}

float InputManager::GetControllerAxisSafe(Input::Gamepad::Axis axis, Input::ControllerID jid)
{
	int count;
	float const* axes = glfwGetJoystickAxes(static_cast<int>(jid), &count);
	if (count > static_cast<int>(axis))
		return axes[static_cast<int>(axis)];
	else
		throw InputException("Controller (" + std::to_string(static_cast<int>(jid)) + ") is not connected, the axis ("
			+ std::to_string(static_cast<int>(axis)) + ") is not supported, or a GLFW error occurred.");
}

bool InputManager::IsControllerConnected(Input::ControllerID jid)
{
	return static_cast<bool>(glfwJoystickPresent(static_cast<int>(jid))) == GLFW_TRUE;
}

bool InputManager::IsControllerGamepad(Input::ControllerID jid)
{
	return static_cast<bool>(glfwJoystickIsGamepad(static_cast<int>(jid))) == GLFW_TRUE;
}

Input::Gamepad::HatDirection InputManager::GetDPad(Input::ControllerID jid, int dpad)
{
	int count;
	return static_cast<Input::Gamepad::HatDirection>(glfwGetJoystickHats(static_cast<int>(jid), &count)[dpad]);
}

Input::Gamepad::HatDirection InputManager::GetDPadSafe(Input::ControllerID jid, int dpad)
{
	int count;
	unsigned char const* hats = glfwGetJoystickHats(static_cast<int>(jid), &count);
	if (count > dpad)
		return static_cast<Input::Gamepad::HatDirection>(hats[dpad]);
	else
		throw InputException("Controller (" + std::to_string(static_cast<int>(jid)) + ") is not connected, the dpad ("
			+ std::to_string(static_cast<int>(dpad)) + ") is not supported, or a GLFW error occurred.");
}

Input::Gamepad::State InputManager::GetFullGamepadState(Input::ControllerID jid)
{
	GLFWgamepadstate state;
	glfwGetGamepadState(static_cast<int>(jid), &state);
	return Input::Gamepad::State(state);
}

Input::Gamepad::State InputManager::GetFullGamepadStateSafe(Input::ControllerID jid)
{
	GLFWgamepadstate state;
	if (glfwGetGamepadState(static_cast<int>(jid), &state))
		return Input::Gamepad::State(state);
	else
		throw InputException("Controller (" + std::to_string(static_cast<int>(jid)) + ") is not connected, or a GLFW error occurred.");
}

bool InputManager::LoadGamepadMapping(const char* mapping)
{
	return static_cast<bool>(glfwUpdateGamepadMappings(mapping));
}
