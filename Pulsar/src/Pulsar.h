#pragma once

#include "Typedefs.h"

struct GLFWwindow;

namespace Pulsar
{
	int StartUp(GLFWwindow*&);
	void Terminate();

	extern real drawTime;
	extern real deltaDrawTime;
	extern real prevDrawTime;
	extern real totalDrawTime;

	void PostInit(void(*post_init)());
	void Run(GLFWwindow*);
	void FrameStart(void(*frame_start)());
}
