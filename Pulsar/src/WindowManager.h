#pragma once

#include <unordered_map>

#include "Window.h"
#include "utils/Functor.inl"

namespace WindowManager
{
	bool RegisterWindow(WindowHandle, Window&&);
	Window* GetWindow(WindowHandle);
	Window* _GetWindow(GLFWwindow*);
	bool _GetHandle(GLFWwindow*, WindowHandle&);
	// TODO non-const Window reference for func?
	void IterateOverWindows(const Functor<void, const Window&>& func);
}
