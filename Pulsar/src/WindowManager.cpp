#include "WindowManager.h"

std::unordered_map<WindowHandle, Window> windows;

bool WindowManager::RegisterWindow(WindowHandle handle, Window&& window)
{
	auto iter = windows.find(handle);
	if (iter == windows.end())
	{
		windows.insert({ handle, std::move(window) });
		return true;
	}
	return false;
}

Window* WindowManager::GetWindow(WindowHandle handle)
{
	auto iter = windows.find(handle);
	return iter != windows.end() ? &iter->second : nullptr;
}
