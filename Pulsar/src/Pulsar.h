#pragma once

// TODO rename real to something more intuitive. Currently, it is used for time and delta time values.
#if PULSAR_DELTA_USE_DOUBLE_PRECISION == 1
typedef double real;
#else
typedef float real;
#endif

#include <GL/glew.h>

typedef GLint TextureSlot;

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
