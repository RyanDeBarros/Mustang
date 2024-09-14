#pragma once

#include <unordered_map>

#include "Window.h"

typedef unsigned char WindowHandle;

namespace WindowManager
{
	bool RegisterWindow(WindowHandle, Window&&);
	Window* GetWindow(WindowHandle);
}
