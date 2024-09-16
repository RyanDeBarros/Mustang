#pragma once

#include <unordered_map>

#include "Window.h"
#include "utils/Functor.inl"

typedef unsigned char WindowHandle;

namespace WindowManager
{
	bool RegisterWindow(WindowHandle, Window&&);
	Window* GetWindow(WindowHandle);
	Window* _GetWindow(GLFWwindow*);
	// TODO non-const Window reference?
	void IterateOverWindows(const Functor<void, const Window&>& func);
}
