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
	EventBucketDispatcher<InputSource::WindowClose, InputEvent::WindowClose, InputBucket::Window> h_WindowClose;
	EventBucketDispatcher<InputSource::WindowContentScale, InputEvent::WindowContentScale, InputBucket::Window> h_WindowContentScale;
	EventBucketDispatcher<InputSource::WindowFocus, InputEvent::WindowFocus, InputBucket::Window> h_WindowFocus;
	EventBucketDispatcher<InputSource::WindowIconify, InputEvent::WindowIconify, InputBucket::Window> h_WindowIconify;
	EventBucketDispatcher<InputSource::WindowMaximize, InputEvent::WindowMaximize, InputBucket::Window> h_WindowMaximize;
	EventBucketDispatcher<InputSource::WindowPos, InputEvent::WindowPos, InputBucket::Window> h_WindowPos;
	EventBucketDispatcher<InputSource::WindowRefresh, InputEvent::WindowRefresh, InputBucket::Window> h_WindowRefresh;
	EventBucketDispatcher<InputSource::WindowResize, InputEvent::WindowResize, InputBucket::Window> h_WindowResize;
	EventBucketDispatcher<InputSource::CursorEnter, InputEvent::CursorEnter, InputBucket::Window> h_CursorEnter;
	EventBucketDispatcher<InputSource::CursorPos, InputEvent::CursorPos, InputBucket::Window> h_CursorPos;
	EventBucketDispatcher<InputSource::PathDrop, InputEvent::PathDrop, InputBucket::Window> h_PathDrop;
	EventBucketDispatcher<InputSource::ControllerConnect, InputEvent::ControllerConnect, InputBucket::ControllerConnect> h_ControllerConnect;
	EventBucketDispatcher<InputSource::Key, InputEvent::Key, InputBucket::Key> h_Key;
	EventBucketDispatcher<InputSource::MonitorConnect, InputEvent::MonitorConnect, InputBucket::MonitorConnect> h_MonitorConnect;
	EventBucketDispatcher<InputSource::MouseButton, InputEvent::MouseButton, InputBucket::MouseButton> h_MouseButton;
	EventBucketDispatcher<InputSource::Scroll, InputEvent::Scroll, InputBucket::Window> h_Scroll;

public:
	EventBucketDispatcher<InputSource::WindowClose, InputEvent::WindowClose, InputBucket::Window>& DispatchWindowClose() { return h_WindowClose; }
	EventBucketDispatcher<InputSource::WindowContentScale, InputEvent::WindowContentScale, InputBucket::Window>& DispatchWindowContentScale() { return h_WindowContentScale; }
	EventBucketDispatcher<InputSource::WindowFocus, InputEvent::WindowFocus, InputBucket::Window>& DispatchWindowFocus() { return h_WindowFocus; }
	EventBucketDispatcher<InputSource::WindowIconify, InputEvent::WindowIconify, InputBucket::Window>& DispatchWindowIconify() { return h_WindowIconify; }
	EventBucketDispatcher<InputSource::WindowMaximize, InputEvent::WindowMaximize, InputBucket::Window>& DispatchWindowMaximize() { return h_WindowMaximize; }
	EventBucketDispatcher<InputSource::WindowPos, InputEvent::WindowPos, InputBucket::Window>& DispatchWindowPos() { return h_WindowPos; }
	EventBucketDispatcher<InputSource::WindowRefresh, InputEvent::WindowRefresh, InputBucket::Window>& DispatchWindowRefresh() { return h_WindowRefresh; }
	EventBucketDispatcher<InputSource::WindowResize, InputEvent::WindowResize, InputBucket::Window>& DispatchWindowResize() { return h_WindowResize; }
	EventBucketDispatcher<InputSource::CursorEnter, InputEvent::CursorEnter, InputBucket::Window>& DispatchCursorEnter() { return h_CursorEnter; }
	EventBucketDispatcher<InputSource::CursorPos, InputEvent::CursorPos, InputBucket::Window>& DispatchCursorPos() { return h_CursorPos; }
	EventBucketDispatcher<InputSource::PathDrop, InputEvent::PathDrop, InputBucket::Window>& DispatchPathDrop() { return h_PathDrop; }
	EventBucketDispatcher<InputSource::ControllerConnect, InputEvent::ControllerConnect, InputBucket::ControllerConnect>& DispatchControllerConnect() { return h_ControllerConnect; }
	EventBucketDispatcher<InputSource::Key, InputEvent::Key, InputBucket::Key>& DispatchKey() { return h_Key; }
	EventBucketDispatcher<InputSource::MonitorConnect, InputEvent::MonitorConnect, InputBucket::MonitorConnect>& DispatchMonitorConnect() { return h_MonitorConnect; }
	EventBucketDispatcher<InputSource::MouseButton, InputEvent::MouseButton, InputBucket::MouseButton>& DispatchMouseButton() { return h_MouseButton; }
	EventBucketDispatcher<InputSource::Scroll, InputEvent::Scroll, InputBucket::Window>& DispatchScroll() { return h_Scroll; }
};
