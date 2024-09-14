#pragma once

#include <stdexcept>

#include "VendorInclude.h"

// TODO use std::runtime_error instead of std::exception
struct WindowException : public std::runtime_error
{
	WindowException(const std::string& err_msg = "Window error occured.") : std::runtime_error(err_msg) {}
};

class Window
{
	// TODO have Renderer as a data member? so it isn't a singleton?
	GLFWwindow* window;
	unsigned int width, height;

public:
	Window(unsigned int width, unsigned int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);

	unsigned int GetWidth() const { return width; }
	unsigned int GetHeight() const { return height; }

	void Focus() const;
	void ForceRefresh() const;
	bool ShouldNotClose() const { return !glfwWindowShouldClose(window); }
};
