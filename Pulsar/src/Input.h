#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "VendorInclude.h"
#include "Handles.inl"
#include "utils/Functor.inl"

namespace Input
{
	enum class Key
	{
		A = GLFW_KEY_A,
		B = GLFW_KEY_B,
		C = GLFW_KEY_C,
		D = GLFW_KEY_D,
		E = GLFW_KEY_E,
		F = GLFW_KEY_F,
		G = GLFW_KEY_G,
		H = GLFW_KEY_H,
		I = GLFW_KEY_I,
		J = GLFW_KEY_J,
		K = GLFW_KEY_K,
		L = GLFW_KEY_L,
		M = GLFW_KEY_M,
		N = GLFW_KEY_N,
		O = GLFW_KEY_O,
		P = GLFW_KEY_P,
		Q = GLFW_KEY_Q,
		R = GLFW_KEY_R,
		S = GLFW_KEY_S,
		T = GLFW_KEY_T,
		U = GLFW_KEY_U,
		V = GLFW_KEY_V,
		W = GLFW_KEY_W,
		X = GLFW_KEY_X,
		Y = GLFW_KEY_Y,
		Z = GLFW_KEY_Z,
		SPACE = GLFW_KEY_SPACE,
		ROW_0 = GLFW_KEY_0,
		ROW_1 = GLFW_KEY_1,
		ROW_2 = GLFW_KEY_2,
		ROW_3 = GLFW_KEY_3,
		ROW_4 = GLFW_KEY_4,
		ROW_5 = GLFW_KEY_5,
		ROW_6 = GLFW_KEY_6,
		ROW_7 = GLFW_KEY_7,
		ROW_8 = GLFW_KEY_8,
		ROW_9 = GLFW_KEY_9,
		NP_0 = GLFW_KEY_KP_0,
		NP_1 = GLFW_KEY_KP_1,
		NP_2 = GLFW_KEY_KP_2,
		NP_3 = GLFW_KEY_KP_3,
		NP_4 = GLFW_KEY_KP_4,
		NP_5 = GLFW_KEY_KP_5,
		NP_6 = GLFW_KEY_KP_6,
		NP_7 = GLFW_KEY_KP_7,
		NP_8 = GLFW_KEY_KP_8,
		NP_9 = GLFW_KEY_KP_9,
		CONTROL_LEFT = GLFW_KEY_LEFT_CONTROL,
		CONTROL_RIGHT = GLFW_KEY_RIGHT_CONTROL,
		ALT_LEFT = GLFW_KEY_LEFT_ALT,
		ALT_RIGHT = GLFW_KEY_RIGHT_ALT,
		SHIFT_LEFT = GLFW_KEY_LEFT_SHIFT,
		SHIFT_RIGHT = GLFW_KEY_RIGHT_SHIFT,
		SUPER_LEFT = GLFW_KEY_LEFT_SUPER,
		SUPER_RIGHT = GLFW_KEY_RIGHT_SUPER,
		BRACKET_LEFT = GLFW_KEY_LEFT_BRACKET,
		BRACKET_RIGHT = GLFW_KEY_RIGHT_BRACKET,
		ENTER = GLFW_KEY_ENTER,
		TAB = GLFW_KEY_TAB,
		CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
		NUM_LOCK = GLFW_KEY_NUM_LOCK,
		PERIOD = GLFW_KEY_PERIOD,
		COMMA = GLFW_KEY_COMMA,
		APOSTROPHE = GLFW_KEY_APOSTROPHE,
		SEMICOLON = GLFW_KEY_SEMICOLON,
		SLASH = GLFW_KEY_SLASH,
		BACKSLASH = GLFW_KEY_BACKSLASH,
		BACKTICK = GLFW_KEY_GRAVE_ACCENT,
		BACKSPACE = GLFW_KEY_BACKSPACE,
		ESCAPE = GLFW_KEY_ESCAPE,
		DELETE = GLFW_KEY_DELETE,
		INSERT = GLFW_KEY_INSERT,
		HOME = GLFW_KEY_HOME,
		END = GLFW_KEY_END,
		PAGE_UP = GLFW_KEY_PAGE_UP,
		PAGE_DOWN = GLFW_KEY_PAGE_DOWN,
		SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK,
		PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,
		PAUSE = GLFW_KEY_PAUSE,
		MENU = GLFW_KEY_MENU,
		F1 = GLFW_KEY_F1,
		F2 = GLFW_KEY_F2,
		F3 = GLFW_KEY_F3,
		F4 = GLFW_KEY_F4,
		F5 = GLFW_KEY_F5,
		F6 = GLFW_KEY_F6,
		F7 = GLFW_KEY_F7,
		F8 = GLFW_KEY_F8,
		F9 = GLFW_KEY_F9,
		F10 = GLFW_KEY_F10,
		F11 = GLFW_KEY_F11,
		F12 = GLFW_KEY_F12,
		F13 = GLFW_KEY_F13,
		F14 = GLFW_KEY_F14,
		F15 = GLFW_KEY_F15,
		F16 = GLFW_KEY_F16,
		F17 = GLFW_KEY_F17,
		F18 = GLFW_KEY_F18,
		F19 = GLFW_KEY_F19,
		F20 = GLFW_KEY_F20,
		F21 = GLFW_KEY_F21,
		F22 = GLFW_KEY_F22,
		F23 = GLFW_KEY_F23,
		F24 = GLFW_KEY_F24,
		F25 = GLFW_KEY_F25,
		MINUS = GLFW_KEY_MINUS,
		EQUAL = GLFW_KEY_EQUAL,
		NP_PLUS = GLFW_KEY_KP_ADD,
		NP_MINUS = GLFW_KEY_KP_SUBTRACT,
		NP_PERIOD = GLFW_KEY_KP_DECIMAL,
		NP_STAR = GLFW_KEY_KP_MULTIPLY,
		NP_SLASH = GLFW_KEY_KP_DIVIDE,
		NP_ENTER = GLFW_KEY_KP_ENTER,
		NP_EQUAL = GLFW_KEY_KP_EQUAL,
		LEFT = GLFW_KEY_LEFT,
		UP = GLFW_KEY_UP,
		RIGHT = GLFW_KEY_RIGHT,
		DOWN = GLFW_KEY_DOWN,
		UNKNOWN = GLFW_KEY_UNKNOWN,
		WORLD_1 = GLFW_KEY_WORLD_1,
		WORLD_2 = GLFW_KEY_WORLD_2,
	};

	enum class MouseButton
	{
		LEFT = GLFW_MOUSE_BUTTON_LEFT,
		RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
		MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
		EXTRA_1 = GLFW_MOUSE_BUTTON_4,
		EXTRA_2 = GLFW_MOUSE_BUTTON_5,
		EXTRA_3 = GLFW_MOUSE_BUTTON_6,
		EXTRA_4 = GLFW_MOUSE_BUTTON_7,
		EXTRA_5 = GLFW_MOUSE_BUTTON_8
	};

	enum class Action
	{
		PRESS = GLFW_PRESS,
		RELEASE = GLFW_RELEASE,
		REPEAT = GLFW_REPEAT
	};

	enum class ControllerID
	{
		J1 = GLFW_JOYSTICK_1,
		J2 = GLFW_JOYSTICK_2,
		J3 = GLFW_JOYSTICK_3,
		J4 = GLFW_JOYSTICK_4,
		J5 = GLFW_JOYSTICK_5,
		J6 = GLFW_JOYSTICK_6,
		J7 = GLFW_JOYSTICK_7,
		J8 = GLFW_JOYSTICK_8,
		J9 = GLFW_JOYSTICK_9,
		J10 = GLFW_JOYSTICK_10,
		J11 = GLFW_JOYSTICK_11,
		J12 = GLFW_JOYSTICK_12,
		J13 = GLFW_JOYSTICK_13,
		J14 = GLFW_JOYSTICK_14,
		J15 = GLFW_JOYSTICK_15,
		J16 = GLFW_JOYSTICK_16
	};

	namespace Gamepad
	{
		enum class Button
		{
			FACE_UP = GLFW_GAMEPAD_BUTTON_TRIANGLE,
			FACE_RIGHT = GLFW_GAMEPAD_BUTTON_CIRCLE,
			FACE_DOWN = GLFW_GAMEPAD_BUTTON_CROSS,
			FACE_LEFT = GLFW_GAMEPAD_BUTTON_SQUARE,
			DPAD_UP = GLFW_GAMEPAD_BUTTON_DPAD_UP,
			DPAD_RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
			DPAD_DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
			DPAD_LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
			START = GLFW_GAMEPAD_BUTTON_START,
			SELECT = GLFW_GAMEPAD_BUTTON_GUIDE,
			BACK = GLFW_GAMEPAD_BUTTON_BACK,
			LEFT_BUMPER = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
			RIGHT_BUMPER = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
			LEFT_STICK = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
			RIGHT_STICK = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB
		};
		enum class Axis
		{
			LEFT_X = GLFW_GAMEPAD_AXIS_LEFT_X,
			LEFT_Y = GLFW_GAMEPAD_AXIS_LEFT_Y,
			RIGHT_X = GLFW_GAMEPAD_AXIS_RIGHT_X,
			RIGHT_Y = GLFW_GAMEPAD_AXIS_RIGHT_Y,
			LEFT_TRIGGER = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
			RIGHT_TRIGGER = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER
		};
	}

	enum class Mod
	{
		SHIFT = GLFW_MOD_SHIFT,
		CONTROL = GLFW_MOD_CONTROL,
		ALT = GLFW_MOD_ALT,
		SUPER = GLFW_MOD_SUPER,
		CAPS_LOCK = GLFW_MOD_CAPS_LOCK,
		NUM_LOCK = GLFW_MOD_NUM_LOCK
	};
}

constexpr int operator&(int x, Input::Mod y)
{
	return x & static_cast<int>(y);
}

constexpr int operator&(Input::Mod x, int y)
{
	return static_cast<int>(x) & y;
}

constexpr int operator&(Input::Mod x, Input::Mod y)
{
	return static_cast<int>(x) & static_cast<int>(y);
}

namespace InputBucket
{
	struct Window
	{
		WindowHandle window;
		Window(WindowHandle window = 0) : window(window) {}
		bool operator==(const InputBucket::Window& other) const { return window == other.window; }
	};
	struct ControllerConnect
	{
		Input::ControllerID jid;
		bool connected;
		ControllerConnect(Input::ControllerID jid, bool connected) : jid(jid), connected(connected) {}
		bool operator==(const InputBucket::ControllerConnect& other) const { return jid == other.jid && connected == other.connected; }
	};
	struct Key
	{
		WindowHandle window;
		Input::Key key;
		Input::Action action;
		Key(WindowHandle window, Input::Key key, Input::Action action) : window(window), key(key), action(action) {}
		bool operator==(const InputBucket::Key& other) const { return window == other.window && key == other.key && action == other.action; }
	};
	struct MonitorConnect
	{
		GLFWmonitor* monitor = nullptr;
		bool connected;
		MonitorConnect(GLFWmonitor* monitor, bool connected) : monitor(monitor), connected(connected) {}
		bool operator==(const InputBucket::MonitorConnect& other) const { return monitor == other.monitor && connected == other.connected; }
	};
	struct MouseButton
	{
		WindowHandle window;
		Input::MouseButton button;
		Input::Action action;
		MouseButton(WindowHandle window, Input::MouseButton button, Input::Action action) : window(window), button(button), action(action) {}
		bool operator==(const InputBucket::MouseButton& other) const { return window == other.window && button == other.button && action == other.action; }
	};
}

template<>
struct std::hash<InputBucket::Window>
{
	size_t operator()(const InputBucket::Window& window) const
	{
		return std::hash<WindowHandle>{}(window.window);
	}
};
template<>
struct std::hash<InputBucket::ControllerConnect>
{
	size_t operator()(const InputBucket::ControllerConnect& cc) const
	{
		return std::hash<Input::ControllerID>{}(cc.jid) ^ (std::hash<bool>{}(cc.connected) << 1);
	}
};
template<>
struct std::hash<InputBucket::Key>
{
	size_t operator()(const InputBucket::Key& key) const
	{
		return std::hash<WindowHandle>{}(key.window) ^ (std::hash<Input::Key>{}(key.key) << 1) ^ (std::hash<Input::Action>{}(key.action) << 2);
	}
};
template<>
struct std::hash<InputBucket::MonitorConnect>
{
	size_t operator()(const InputBucket::MonitorConnect& mon) const
	{
		return std::hash<GLFWmonitor*>{}(mon.monitor) ^ (std::hash<bool>{}(mon.connected) << 1);
	}
};
template<>
struct std::hash<InputBucket::MouseButton>
{
	size_t operator()(const InputBucket::MouseButton& mb) const
	{
		return std::hash<WindowHandle>{}(mb.window) ^ (std::hash<Input::MouseButton>{}(mb.button) << 1) ^ (std::hash<Input::Action>{}(mb.action) << 2);
	}
};

namespace InputEvent
{
	struct WindowClose
	{
	};
	struct WindowContentScale
	{
		float sx;
		float sy;
	};
	struct WindowFocus
	{
		bool focused;
	};
	struct WindowIconify
	{
		bool iconified;
	};
	struct WindowMaximize
	{
		bool maximized;
	};
	struct WindowPos
	{
		int x;
		int y;
	};
	struct WindowRefresh
	{
	};
	struct WindowResize
	{
		int width;
		int height;
	};
	struct CursorEnter
	{
		bool entered;
	};
	struct CursorPos
	{
		float x;
		float y;
	};
	struct PathDrop
	{
		std::vector<std::string> paths;
	};
	struct ControllerConnect
	{
	};
	struct Key
	{
		int scancode;
		int mods;
	};
	struct MonitorConnect
	{
	};
	struct MouseButton
	{
		int mods;
	};
	struct Scroll
	{
		double xoff;
		double yoff;
	};
}

template<typename IS, typename IE, typename ID>
using input_separator = Functor<std::pair<IS, IE>, const ID&>;

namespace InputSource
{
	struct WindowClose
	{
		WindowHandle window;
		using sep_t = input_separator<InputBucket::Window, InputEvent::WindowClose, InputSource::WindowClose>;
		static const sep_t Separator;
	};
	inline const InputSource::WindowClose::sep_t InputSource::WindowClose::Separator = [](const InputSource::WindowClose& d)
		{ return std::make_pair(InputBucket::Window{ d.window }, InputEvent::WindowClose{}); };
	struct WindowContentScale
	{
		WindowHandle window;
		float sx;
		float sy;
		using sep_t = input_separator<InputBucket::Window, InputEvent::WindowContentScale, InputSource::WindowContentScale>;
		static const sep_t Separator;
	};
	inline const InputSource::WindowContentScale::sep_t InputSource::WindowContentScale::Separator = [](const InputSource::WindowContentScale& d)
		{ return std::make_pair(InputBucket::Window{ d.window }, InputEvent::WindowContentScale{ d.sx, d.sy }); };
	struct WindowFocus
	{
		WindowHandle window;
		bool focused;
		using sep_t = input_separator<InputBucket::Window, InputEvent::WindowFocus, InputSource::WindowFocus>;
		static const sep_t Separator;
	};
	inline const InputSource::WindowFocus::sep_t InputSource::WindowFocus::Separator = [](const InputSource::WindowFocus& d)
		{ return std::make_pair(InputBucket::Window{ d.window }, InputEvent::WindowFocus{ d.focused }); };
	struct WindowIconify
	{
		WindowHandle window;
		bool iconified;
		using sep_t = input_separator<InputBucket::Window, InputEvent::WindowIconify, InputSource::WindowIconify>;
		static const sep_t Separator;
	};
	inline const InputSource::WindowIconify::sep_t InputSource::WindowIconify::Separator = [](const InputSource::WindowIconify& d)
		{ return std::make_pair(InputBucket::Window{ d.window }, InputEvent::WindowIconify{ d.iconified }); };
	struct WindowMaximize
	{
		WindowHandle window;
		bool maximized;
		using sep_t = input_separator<InputBucket::Window, InputEvent::WindowMaximize, InputSource::WindowMaximize>;
		static const sep_t Separator;
	};
	inline const InputSource::WindowMaximize::sep_t InputSource::WindowMaximize::Separator = [](const InputSource::WindowMaximize& d)
		{ return std::make_pair(InputBucket::Window{ d.window }, InputEvent::WindowMaximize{ d.maximized }); };
	struct WindowPos
	{
		WindowHandle window;
		int x;
		int y;
		using sep_t = input_separator<InputBucket::Window, InputEvent::WindowPos, InputSource::WindowPos>;
		static const sep_t Separator;
	};
	inline const InputSource::WindowPos::sep_t InputSource::WindowPos::Separator = [](const InputSource::WindowPos& d)
		{ return std::make_pair(InputBucket::Window{ d.window }, InputEvent::WindowPos{ d.x, d.y }); };
	struct WindowRefresh
	{
		WindowHandle window;
		using sep_t = input_separator<InputBucket::Window, InputEvent::WindowRefresh, InputSource::WindowRefresh>;
		static const sep_t Separator;
	};
	inline const InputSource::WindowRefresh::sep_t InputSource::WindowRefresh::Separator = [](const InputSource::WindowRefresh& d)
		{ return std::make_pair(InputBucket::Window{ d.window }, InputEvent::WindowRefresh{}); };
	struct WindowResize
	{
		WindowHandle window;
		int width;
		int height;
		using sep_t = input_separator<InputBucket::Window, InputEvent::WindowResize, InputSource::WindowResize>;
		static const sep_t Separator;
	};
	inline const InputSource::WindowResize::sep_t InputSource::WindowResize::Separator = [](const InputSource::WindowResize& d)
		{ return std::make_pair(InputBucket::Window{ d.window }, InputEvent::WindowResize{ d.width, d.height }); };
	struct CursorEnter
	{
		WindowHandle window;
		bool entered;
		using sep_t = input_separator<InputBucket::Window, InputEvent::CursorEnter, InputSource::CursorEnter>;
		static const sep_t Separator;
	};
	inline const InputSource::CursorEnter::sep_t InputSource::CursorEnter::Separator = [](const InputSource::CursorEnter& d)
		{ return std::make_pair(InputBucket::Window{ d.window }, InputEvent::CursorEnter{ d.entered }); };
	struct CursorPos
	{
		WindowHandle window;
		float x;
		float y;
		using sep_t = input_separator<InputBucket::Window, InputEvent::CursorPos, InputSource::CursorPos>;
		static const sep_t Separator;
	};
	inline const InputSource::CursorPos::sep_t InputSource::CursorPos::Separator = [](const InputSource::CursorPos& d)
		{ return std::make_pair(InputBucket::Window{ d.window }, InputEvent::CursorPos{ d.x, d.y }); };
	struct PathDrop
	{
		WindowHandle window;
		std::vector<std::string> paths;
		using sep_t = input_separator<InputBucket::Window, InputEvent::PathDrop, InputSource::PathDrop>;
		static const sep_t Separator;
	};
	inline const InputSource::PathDrop::sep_t InputSource::PathDrop::Separator = [](const InputSource::PathDrop& d)
		{ return std::make_pair(InputBucket::Window{ d.window }, InputEvent::PathDrop{ d.paths }); };
	struct ControllerConnect
	{
		Input::ControllerID jid;
		bool connected;
		using sep_t = input_separator<InputBucket::ControllerConnect, InputEvent::ControllerConnect, InputSource::ControllerConnect>;
		static const sep_t Separator;
	};
	inline const InputSource::ControllerConnect::sep_t InputSource::ControllerConnect::Separator = [](const InputSource::ControllerConnect& d)
		{ return std::make_pair(InputBucket::ControllerConnect{ d.jid, d.connected }, InputEvent::ControllerConnect{}); };
	struct Key
	{
		WindowHandle window;
		Input::Key key;
		Input::Action action;
		int scancode;
		int mods;
		using sep_t = input_separator<InputBucket::Key, InputEvent::Key, InputSource::Key>;
		static const sep_t Separator;
	};
	inline const InputSource::Key::sep_t InputSource::Key::Separator = [](const InputSource::Key& d)
		{ return std::make_pair(InputBucket::Key{ d.window, d.key, d.action }, InputEvent::Key{ d.scancode, d.mods }); };
	struct MonitorConnect
	{
		GLFWmonitor* monitor = nullptr;
		bool connected;
		using sep_t = input_separator<InputBucket::MonitorConnect, InputEvent::MonitorConnect, InputSource::MonitorConnect>;
		static const sep_t Separator;
	};
	inline const InputSource::MonitorConnect::sep_t InputSource::MonitorConnect::Separator = [](const InputSource::MonitorConnect& d)
		{ return std::make_pair(InputBucket::MonitorConnect{ d.monitor, d.connected }, InputEvent::MonitorConnect{}); };
	struct MouseButton
	{
		WindowHandle window;
		Input::MouseButton button;
		Input::Action action;
		int mods;
		using sep_t = input_separator<InputBucket::MouseButton, InputEvent::MouseButton, InputSource::MouseButton>;
		static const sep_t Separator;
	};
	inline const InputSource::MouseButton::sep_t InputSource::MouseButton::Separator = [](const InputSource::MouseButton& d)
		{ return std::make_pair(InputBucket::MouseButton{ d.window, d.button, d.action }, InputEvent::MouseButton{ d.mods }); };
	struct Scroll
	{
		WindowHandle window;
		double xoff;
		double yoff;
		using sep_t = input_separator<InputBucket::Window, InputEvent::Scroll, InputSource::Scroll>;
		static const sep_t Separator;
	};
	inline const InputSource::Scroll::sep_t InputSource::Scroll::Separator = [](const InputSource::Scroll& d)
		{ return std::make_pair(InputBucket::Window{ d.window }, InputEvent::Scroll{ d.xoff, d.yoff }); };
}
