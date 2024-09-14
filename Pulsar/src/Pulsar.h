#pragma once

// TODO rename real to something more intuitive. Currently, it is used for time and delta time values.
#if PULSAR_DELTA_USE_DOUBLE_PRECISION == 1
typedef double real;
#else
typedef float real;
#endif

#include "WindowManager.h"

typedef GLint TextureSlot;

struct GLFWwindow;

namespace Pulsar
{
	void CreateWindow(WindowHandle handle, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
	int StartUp(const char* title);
	void Terminate();

	extern real drawTime;
	extern real deltaDrawTime;
	extern real prevDrawTime;
	extern real totalDrawTime;

	// #3
	void Run();
	void PostInit(void(*post_init)());
	void FrameStart(void(*frame_start)());

	void _ExecFrame();
}
