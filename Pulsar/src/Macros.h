#pragma once

/******************************/
#ifdef _DEBUG
#define PULSAR_DEBUGGING_MODE 1
#else
#define PULSAR_DEBUGGING_MODE 0
#endif
#define PULSAR_ASSUME_INITIALIZED 1
#define PULSAR_IGNORE_WARNINGS_NULL_UNIFORM_LEXICON 1
#define PULSAR_IGNORE_WARNINGS_NULL_TEXTURE 1
#define PULSAR_IGNORE_WARNINGS_NULL_SHADER 1
#define PULSAR_IGNORE_WARNINGS_NULL_TILE 1
#define PULSAR_DELTA_USE_DOUBLE_PRECISION 0
/******************************/

#define ASSERT(x) if (!(x)) __debugbreak();
#if PULSAR_DEBUGGING_MODE == 1
#	define TRY(x) ASSERT(glNoError(#x, __FILE__, __LINE__)) x; ASSERT(glNoError(#x, __FILE__, __LINE__))
#else
#	define TRY(x) x;
#endif

#include <GL/glew.h>

#include <string>

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
