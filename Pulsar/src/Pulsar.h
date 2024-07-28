#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

	void Run(GLFWwindow*);
}
