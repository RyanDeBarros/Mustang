#pragma once

#include <glm/glm.hpp>
#include <stdexcept>

#include "VendorInclude.h"
#include "utils/Functor.inl"

// TODO use std::runtime_error instead of std::exception
struct WindowException : public std::runtime_error
{
	WindowException(const std::string& err_msg = "Window error occured.") : std::runtime_error(err_msg) {}
};

class Window
{
	// TODO have Renderer as a data member? so it isn't a singleton?
	GLFWwindow* window = nullptr;

public:
	Window() = default;

	Window(unsigned int width, unsigned int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);

	unsigned int GetWidth() const { int width; glfwGetWindowSize(window, &width, &width); return width; }
	unsigned int GetHeight() const { int height; glfwGetWindowSize(window, &height, &height); return height; }
	glm::ivec2 GetSize() const { int width; int height; glfwGetWindowSize(window, &width, &height); return { width, height }; }
	void OperateWindow(const Functor<void, GLFWwindow&>& op) { op(*window); }

	void Focus() const;
	void _ForceRefresh() const;
	bool ShouldNotClose() const { return !glfwWindowShouldClose(window); }
	GLFWwindow* _GetInternal() { return window; }

	void _CallbackWindowRefresh();
	void _CallbackWindowResize(int width, int height);
	void _CallbackWindowClose();
	void _CallbackWindowContentScale(float sx, float sy);
	void _CallbackWindowFocus(bool focused);
	void _CallbackWindowIconify(bool iconified);
	void _CallbackWindowMaximize(bool maximized);
	void _CallbackWindowPos(int x, int y);
	void _CallbackCursorEnter(bool entered);
	void _CallbackCursorPos(double x, double y);
	void _CallbackPathDrop(int path_count, const char** paths);
	void _CallbackKey(int key, int scancode, int action, int mods);
	void _CallbackMouseButton(int button, int action, int mods);
	void _CallbackScroll(double xoff, double yoff);
};
