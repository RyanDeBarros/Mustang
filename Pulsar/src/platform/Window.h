#pragma once

#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>

#include "VendorInclude.h"
#include "utils/Functor.inl"
#include "Input.h"
#include "Cursor.h"

// TODO use std::runtime_error instead of std::exception
struct WindowException : public std::runtime_error
{
	WindowException(const std::string& err_msg = "Window error occured.") : std::runtime_error(err_msg) {}
};

class Window
{
	// TODO have Renderer as a data member? so it isn't a singleton?
	GLFWwindow* window = nullptr;
	Cursor cursor;

public:
	Window() = default;

	Window(unsigned int width, unsigned int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);

	unsigned int GetWidth() const { int width; glfwGetWindowSize(window, &width, &width); return width; }
	unsigned int GetHeight() const { int height; glfwGetWindowSize(window, &height, &height); return height; }
	glm::ivec2 GetSize() const { int width; int height; glfwGetWindowSize(window, &width, &height); return { width, height }; }
	void OperateWindow(const Functor<void, GLFWwindow&>& op) { op(*window); }

	void _Register(WindowHandle);
	void Focus() const;
	void _ForceRefresh() const;
	bool ShouldNotClose() const { return !glfwWindowShouldClose(window); }
	GLFWwindow* _GetInternal() { return window; }

	void SetCursor(Cursor&& cursor);
	glm::vec2 GetCursorPos() const;
	void SetCursorPos(const glm::vec2& pos);
};
