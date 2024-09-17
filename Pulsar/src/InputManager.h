#pragma once

#include "WindowManager.h"
#include "EventDispatcher.inl"

class InputManager
{
	InputManager();
	InputManager(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	~InputManager() = default;

public:
	static InputManager& Instance()
	{
		static InputManager instance;
		return instance;
	}

	void AssignWindowCallbacks(Window&);

private:
	EventBucketDispatcher<InputData::WindowClose, InputEvent::WindowClose, InputSource::Window> h_WindowClose;
	EventBucketDispatcher<InputData::WindowContentScale, InputEvent::WindowContentScale, InputSource::Window> h_WindowContentScale;
	EventBucketDispatcher<InputData::WindowFocus, InputEvent::WindowFocus, InputSource::Window> h_WindowFocus;
	EventBucketDispatcher<InputData::WindowIconify, InputEvent::WindowIconify, InputSource::Window> h_WindowIconify;
	EventBucketDispatcher<InputData::WindowMaximize, InputEvent::WindowMaximize, InputSource::Window> h_WindowMaximize;
	EventBucketDispatcher<InputData::WindowPos, InputEvent::WindowPos, InputSource::Window> h_WindowPos;
	EventBucketDispatcher<InputData::WindowRefresh, InputEvent::WindowRefresh, InputSource::Window> h_WindowRefresh;
	EventBucketDispatcher<InputData::WindowResize, InputEvent::WindowResize, InputSource::Window> h_WindowResize;
	EventBucketDispatcher<InputData::CursorEnter, InputEvent::CursorEnter, InputSource::Window> h_CursorEnter;
	EventBucketDispatcher<InputData::CursorPos, InputEvent::CursorPos, InputSource::Window> h_CursorPos;
	EventBucketDispatcher<InputData::PathDrop, InputEvent::PathDrop, InputSource::Window> h_PathDrop;
	EventBucketDispatcher<InputData::ControllerConnect, InputEvent::ControllerConnect, InputSource::ControllerConnect> h_ControllerConnect;
	EventBucketDispatcher<InputData::Key, InputEvent::Key, InputSource::Key> h_Key;
	EventBucketDispatcher<InputData::MonitorConnect, InputEvent::MonitorConnect, InputSource::MonitorConnect> h_MonitorConnect;
	EventBucketDispatcher<InputData::MouseButton, InputEvent::MouseButton, InputSource::MouseButton> h_MouseButton;
	EventBucketDispatcher<InputData::Scroll, InputEvent::Scroll, InputSource::Window> h_Scroll;

public:
	EventBucketDispatcher<InputData::WindowClose, InputEvent::WindowClose, InputSource::Window>& DispatchWindowClose() { return h_WindowClose; }
	EventBucketDispatcher<InputData::WindowContentScale, InputEvent::WindowContentScale, InputSource::Window>& DispatchWindowContentScale() { return h_WindowContentScale; }
	EventBucketDispatcher<InputData::WindowFocus, InputEvent::WindowFocus, InputSource::Window>& DispatchWindowFocus() { return h_WindowFocus; }
	EventBucketDispatcher<InputData::WindowIconify, InputEvent::WindowIconify, InputSource::Window>& DispatchWindowIconify() { return h_WindowIconify; }
	EventBucketDispatcher<InputData::WindowMaximize, InputEvent::WindowMaximize, InputSource::Window>& DispatchWindowMaximize() { return h_WindowMaximize; }
	EventBucketDispatcher<InputData::WindowPos, InputEvent::WindowPos, InputSource::Window>& DispatchWindowPos() { return h_WindowPos; }
	EventBucketDispatcher<InputData::WindowRefresh, InputEvent::WindowRefresh, InputSource::Window>& DispatchWindowRefresh() { return h_WindowRefresh; }
	EventBucketDispatcher<InputData::WindowResize, InputEvent::WindowResize, InputSource::Window>& DispatchWindowResize() { return h_WindowResize; }
	EventBucketDispatcher<InputData::CursorEnter, InputEvent::CursorEnter, InputSource::Window>& DispatchCursorEnter() { return h_CursorEnter; }
	EventBucketDispatcher<InputData::CursorPos, InputEvent::CursorPos, InputSource::Window>& DispatchCursorPos() { return h_CursorPos; }
	EventBucketDispatcher<InputData::PathDrop, InputEvent::PathDrop, InputSource::Window>& DispatchPathDrop() { return h_PathDrop; }
	EventBucketDispatcher<InputData::ControllerConnect, InputEvent::ControllerConnect, InputSource::ControllerConnect>& DispatchControllerConnect() { return h_ControllerConnect; }
	EventBucketDispatcher<InputData::Key, InputEvent::Key, InputSource::Key>& DispatchKey() { return h_Key; }
	EventBucketDispatcher<InputData::MonitorConnect, InputEvent::MonitorConnect, InputSource::MonitorConnect>& DispatchMonitorConnect() { return h_MonitorConnect; }
	EventBucketDispatcher<InputData::MouseButton, InputEvent::MouseButton, InputSource::MouseButton>& DispatchMouseButton() { return h_MouseButton; }
	EventBucketDispatcher<InputData::Scroll, InputEvent::Scroll, InputSource::Window>& DispatchScroll() { return h_Scroll; }
};
