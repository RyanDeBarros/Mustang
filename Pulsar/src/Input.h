#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "VendorInclude.h"
#include "Handles.inl"
#include "utils/Functor.inl"

// TODO make into enum
typedef int ControllerID;

namespace Input
{
	enum class Key
	{
		// TODO
	};

	enum class MouseButton
	{
		Left = GLFW_MOUSE_BUTTON_LEFT,
		Right = GLFW_MOUSE_BUTTON_RIGHT,
		Middle = GLFW_MOUSE_BUTTON_MIDDLE,
		Extra1 = GLFW_MOUSE_BUTTON_4,
		Extra2 = GLFW_MOUSE_BUTTON_5,
		Extra3 = GLFW_MOUSE_BUTTON_6,
		Extra4 = GLFW_MOUSE_BUTTON_7,
		Extra5 = GLFW_MOUSE_BUTTON_8
	};

	enum class Action
	{
		Press = GLFW_PRESS,
		Release = GLFW_RELEASE,
		Repeat = GLFW_REPEAT
	};
}

namespace InputSource
{
	struct Window
	{
		WindowHandle window;
		Window(WindowHandle window = 0) : window(window) {}
		bool operator==(const InputSource::Window& other) const { return window == other.window; }
	};
	struct ControllerConnect
	{
		ControllerID jid;
		bool connected;
		ControllerConnect(ControllerID jid, bool connected) : jid(jid), connected(connected) {}
		bool operator==(const InputSource::ControllerConnect& other) const { return jid == other.jid && connected == other.connected; }
	};
	struct Key
	{
		WindowHandle window;
		Input::Key key;
		Input::Action action;
		Key(WindowHandle window, Input::Key key, Input::Action action) : window(window), key(key), action(action) {}
		bool operator==(const InputSource::Key& other) const { return window == other.window && key == other.key && action == other.action; }
	};
	struct MonitorConnect
	{
		GLFWmonitor* monitor = nullptr;
		bool connected;
		MonitorConnect(GLFWmonitor* monitor, bool connected) : monitor(monitor), connected(connected) {}
		bool operator==(const InputSource::MonitorConnect& other) const { return monitor == other.monitor && connected == other.connected; }
	};
	struct MouseButton
	{
		WindowHandle window;
		Input::MouseButton button;
		Input::Action action;
		MouseButton(WindowHandle window, Input::MouseButton button, Input::Action action) : window(window), button(button), action(action) {}
		bool operator==(const InputSource::MouseButton& other) const { return window == other.window && button == other.button && action == other.action; }
	};
}

template<>
struct std::hash<InputSource::Window>
{
	size_t operator()(const InputSource::Window& window) const
	{
		return std::hash<WindowHandle>{}(window.window);
	}
};
template<>
struct std::hash<InputSource::ControllerConnect>
{
	size_t operator()(const InputSource::ControllerConnect& cc) const
	{
		return std::hash<ControllerID>{}(cc.jid) ^ (std::hash<bool>{}(cc.connected) << 1);
	}
};
template<>
struct std::hash<InputSource::Key>
{
	size_t operator()(const InputSource::Key& key) const
	{
		return std::hash<WindowHandle>{}(key.window) ^ (std::hash<Input::Key>{}(key.key) << 1) ^ (std::hash<Input::Action>{}(key.action) << 2);
	}
};
template<>
struct std::hash<InputSource::MonitorConnect>
{
	size_t operator()(const InputSource::MonitorConnect& mon) const
	{
		return std::hash<GLFWmonitor*>{}(mon.monitor) ^ (std::hash<bool>{}(mon.connected) << 1);
	}
};
template<>
struct std::hash<InputSource::MouseButton>
{
	size_t operator()(const InputSource::MouseButton& mb) const
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

namespace InputData
{
	struct WindowClose
	{
		WindowHandle window;
		using sep_t = input_separator<InputSource::Window, InputEvent::WindowClose, InputData::WindowClose>;
		static const sep_t Separator;
	};
	inline const InputData::WindowClose::sep_t InputData::WindowClose::Separator = make_functor_ptr([](const InputData::WindowClose& d)
		{ return std::make_pair(InputSource::Window{ d.window }, InputEvent::WindowClose{}); });
	struct WindowContentScale
	{
		WindowHandle window;
		float sx;
		float sy;
		using sep_t = input_separator<InputSource::Window, InputEvent::WindowContentScale, InputData::WindowContentScale>;
		static const sep_t Separator;
	};
	inline const InputData::WindowContentScale::sep_t InputData::WindowContentScale::Separator = make_functor_ptr([](const InputData::WindowContentScale& d)
		{ return std::make_pair(InputSource::Window{ d.window }, InputEvent::WindowContentScale{ d.sx, d.sy }); });
	struct WindowFocus
	{
		WindowHandle window;
		bool focused;
		using sep_t = input_separator<InputSource::Window, InputEvent::WindowFocus, InputData::WindowFocus>;
		static const sep_t Separator;
	};
	inline const InputData::WindowFocus::sep_t InputData::WindowFocus::Separator = make_functor_ptr([](const InputData::WindowFocus& d)
		{ return std::make_pair(InputSource::Window{ d.window }, InputEvent::WindowFocus{ d.focused }); });
	struct WindowIconify
	{
		WindowHandle window;
		bool iconified;
		using sep_t = input_separator<InputSource::Window, InputEvent::WindowIconify, InputData::WindowIconify>;
		static const sep_t Separator;
	};
	inline const InputData::WindowIconify::sep_t InputData::WindowIconify::Separator = make_functor_ptr([](const InputData::WindowIconify& d)
		{ return std::make_pair(InputSource::Window{ d.window }, InputEvent::WindowIconify{ d.iconified }); });
	struct WindowMaximize
	{
		WindowHandle window;
		bool maximized;
		using sep_t = input_separator<InputSource::Window, InputEvent::WindowMaximize, InputData::WindowMaximize>;
		static const sep_t Separator;
	};
	inline const InputData::WindowMaximize::sep_t InputData::WindowMaximize::Separator = make_functor_ptr([](const InputData::WindowMaximize& d)
		{ return std::make_pair(InputSource::Window{ d.window }, InputEvent::WindowMaximize{ d.maximized }); });
	struct WindowPos
	{
		WindowHandle window;
		int x;
		int y;
		using sep_t = input_separator<InputSource::Window, InputEvent::WindowPos, InputData::WindowPos>;
		static const sep_t Separator;
	};
	inline const InputData::WindowPos::sep_t InputData::WindowPos::Separator = make_functor_ptr([](const InputData::WindowPos& d)
		{ return std::make_pair(InputSource::Window{ d.window }, InputEvent::WindowPos{ d.x, d.y }); });
	struct WindowRefresh
	{
		WindowHandle window;
		using sep_t = input_separator<InputSource::Window, InputEvent::WindowRefresh, InputData::WindowRefresh>;
		static const sep_t Separator;
	};
	inline const InputData::WindowRefresh::sep_t InputData::WindowRefresh::Separator = make_functor_ptr([](const InputData::WindowRefresh& d)
		{ return std::make_pair(InputSource::Window{ d.window }, InputEvent::WindowRefresh{}); });
	struct WindowResize
	{
		WindowHandle window;
		int width;
		int height;
		using sep_t = input_separator<InputSource::Window, InputEvent::WindowResize, InputData::WindowResize>;
		static const sep_t Separator;
	};
	inline const InputData::WindowResize::sep_t InputData::WindowResize::Separator = make_functor_ptr([](const InputData::WindowResize& d)
		{ return std::make_pair(InputSource::Window{ d.window }, InputEvent::WindowResize{ d.width, d.height }); });
	struct CursorEnter
	{
		WindowHandle window;
		bool entered;
		using sep_t = input_separator<InputSource::Window, InputEvent::CursorEnter, InputData::CursorEnter>;
		static const sep_t Separator;
	};
	inline const InputData::CursorEnter::sep_t InputData::CursorEnter::Separator = make_functor_ptr([](const InputData::CursorEnter& d)
		{ return std::make_pair(InputSource::Window{ d.window }, InputEvent::CursorEnter{ d.entered }); });
	struct CursorPos
	{
		WindowHandle window;
		float x;
		float y;
		using sep_t = input_separator<InputSource::Window, InputEvent::CursorPos, InputData::CursorPos>;
		static const sep_t Separator;
	};
	inline const InputData::CursorPos::sep_t InputData::CursorPos::Separator = make_functor_ptr([](const InputData::CursorPos& d)
		{ return std::make_pair(InputSource::Window{ d.window }, InputEvent::CursorPos{ d.x, d.y }); });
	struct PathDrop
	{
		WindowHandle window;
		std::vector<std::string> paths;
		using sep_t = input_separator<InputSource::Window, InputEvent::PathDrop, InputData::PathDrop>;
		static const sep_t Separator;
	};
	inline const InputData::PathDrop::sep_t InputData::PathDrop::Separator = make_functor_ptr([](const InputData::PathDrop& d)
		{ return std::make_pair(InputSource::Window{ d.window }, InputEvent::PathDrop{ d.paths }); });
	struct ControllerConnect
	{
		ControllerID jid;
		bool connected;
		using sep_t = input_separator<InputSource::ControllerConnect, InputEvent::ControllerConnect, InputData::ControllerConnect>;
		static const sep_t Separator;
	};
	inline const InputData::ControllerConnect::sep_t InputData::ControllerConnect::Separator = make_functor_ptr([](const InputData::ControllerConnect& d)
		{ return std::make_pair(InputSource::ControllerConnect{ d.jid, d.connected }, InputEvent::ControllerConnect{}); });
	struct Key
	{
		WindowHandle window;
		Input::Key key;
		Input::Action action;
		int scancode;
		int mods;
		using sep_t = input_separator<InputSource::Key, InputEvent::Key, InputData::Key>;
		static const sep_t Separator;
	};
	inline const InputData::Key::sep_t InputData::Key::Separator = make_functor_ptr([](const InputData::Key& d)
		{ return std::make_pair(InputSource::Key{ d.window, d.key, d.action }, InputEvent::Key{ d.scancode, d.mods }); });
	struct MonitorConnect
	{
		GLFWmonitor* monitor = nullptr;
		bool connected;
		using sep_t = input_separator<InputSource::MonitorConnect, InputEvent::MonitorConnect, InputData::MonitorConnect>;
		static const sep_t Separator;
	};
	inline const InputData::MonitorConnect::sep_t InputData::MonitorConnect::Separator = make_functor_ptr([](const InputData::MonitorConnect& d)
		{ return std::make_pair(InputSource::MonitorConnect{ d.monitor, d.connected }, InputEvent::MonitorConnect{}); });
	struct MouseButton
	{
		WindowHandle window;
		Input::MouseButton button;
		Input::Action action;
		int mods;
		using sep_t = input_separator<InputSource::MouseButton, InputEvent::MouseButton, InputData::MouseButton>;
		static const sep_t Separator;
	};
	inline const InputData::MouseButton::sep_t InputData::MouseButton::Separator = make_functor_ptr([](const InputData::MouseButton& d)
		{ return std::make_pair(InputSource::MouseButton{ d.window, d.button, d.action }, InputEvent::MouseButton{ d.mods }); });
	struct Scroll
	{
		WindowHandle window;
		double xoff;
		double yoff;
		using sep_t = input_separator<InputSource::Window, InputEvent::Scroll, InputData::Scroll>;
		static const sep_t Separator;
	};
	inline const InputData::Scroll::sep_t InputData::Scroll::Separator = make_functor_ptr([](const InputData::Scroll& d)
		{ return std::make_pair(InputSource::Window{ d.window }, InputEvent::Scroll{ d.xoff, d.yoff }); });
}
