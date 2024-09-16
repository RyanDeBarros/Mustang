#include "WindowManager.h"

std::unordered_map<WindowHandle, Window> windows;
std::unordered_map<GLFWwindow*, WindowHandle> lookup;

bool WindowManager::RegisterWindow(WindowHandle handle, Window&& window)
{
	auto iter = windows.find(handle);
	if (iter == windows.end())
	{
		windows.insert({ handle, std::move(window) });
		lookup.insert({ window._GetInternal(), handle });
		return true;
	}
	return false;
}

Window* WindowManager::GetWindow(WindowHandle handle)
{
	auto iter = windows.find(handle);
	return iter != windows.end() ? &iter->second : nullptr;
}

Window* WindowManager::_GetWindow(GLFWwindow* window)
{
	auto iter = lookup.find(window);
	return iter != lookup.end() ? &windows[iter->second] : nullptr;
}

void WindowManager::IterateOverWindows(const Functor<void, const Window&>& func)
{
	for (auto iter = windows.begin(); iter != windows.end(); ++iter)
		func(iter->second);
}
