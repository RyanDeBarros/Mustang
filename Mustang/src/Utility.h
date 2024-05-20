#pragma once

#define ASSERT(x) if (!(x)) __debugbreak();
#ifdef _DEBUG
#define TRY(x) ASSERT(glNoError(#x)) x; ASSERT(glNoError(#x))
#else
#define TRY(x) x;
#endif

#include "GL/glew.h"

#include "Logger.h"

static bool glNoError(const char* function_name)
{
	bool no_err = true;
	while (GLenum error = glGetError())
	{
		Logger::LogError(error, function_name);
		no_err = false;
	}
	return no_err;
}

namespace Utility {

	const char* ReadFile(const char* filepath);

}
