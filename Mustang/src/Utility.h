#pragma once

#define ASSERT(x) if (!(x)) __debugbreak();
#ifdef _DEBUG
#define TRY(x) ASSERT(glNoError(#x, __FILE__, __LINE__)) x; ASSERT(glNoError(#x, __FILE__, __LINE__))
#else
#define TRY(x) x;
#endif

#include "GL/glew.h"

#include "Logger.h"

static bool glNoError(const char* function_name, const char* file, int line)
{
	bool no_err = true;
	while (GLenum error = glGetError())
	{
		Logger::LogError(error, function_name, file, std::to_string(line).c_str());
		no_err = false;
	}
	return no_err;
}

namespace Utility {

	const char* ReadFile(const char* filepath);

}
