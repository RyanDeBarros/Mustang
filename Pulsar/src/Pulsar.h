#pragma once

// TODO rename real to something more intuitive. Currently, it is used for time and delta time values.
#if PULSAR_DELTA_USE_DOUBLE_PRECISION == 1
typedef double real;
#else
typedef float real;
#endif

#include <memory>

#include "Window.h"

typedef GLint TextureSlot;

struct GLFWwindow;

namespace Pulsar
{
	std::shared_ptr<Window> CreateWindow(const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
	// #1
	int StartUp();
	// #2
	std::shared_ptr<Window> WelcomeWindow(const char* title);
	// #4
	void Terminate();

	extern real drawTime;
	extern real deltaDrawTime;
	extern real prevDrawTime;
	extern real totalDrawTime;

	// #3
	void Run(class Window&);
	void PostInit(void(*post_init)());
	void FrameStart(void(*frame_start)());
}
