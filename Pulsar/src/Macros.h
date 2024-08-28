#pragma once

/******************************/
#ifndef PULSAR_DEBUGGING_MODE
#ifdef _DEBUG
#define PULSAR_DEBUGGING_MODE 1
#else
#define PULSAR_DEBUGGING_MODE 0
#endif // PULSAR_DEBUGGING_MODE
#endif
#ifndef PULSAR_ASSUME_INITIALIZED
#define PULSAR_ASSUME_INITIALIZED 1
#endif
#ifndef PULSAR_IGNORE_WARNINGS_NULL_UNIFORM_LEXICON
#define PULSAR_IGNORE_WARNINGS_NULL_UNIFORM_LEXICON 1
#endif
#ifndef PULSAR_IGNORE_WARNINGS_NULL_TEXTURE
#define PULSAR_IGNORE_WARNINGS_NULL_TEXTURE 1
#endif
#ifndef PULSAR_IGNORE_WARNINGS_NULL_SHADER
#define PULSAR_IGNORE_WARNINGS_NULL_SHADER 1
#endif
#ifndef PULSAR_IGNORE_WARNINGS_NULL_TILE
#define PULSAR_IGNORE_WARNINGS_NULL_TILE 1
#endif
#ifndef PULSAR_DELTA_USE_DOUBLE_PRECISION
#define PULSAR_DELTA_USE_DOUBLE_PRECISION 0
#endif
/******************************/

#ifndef PULSAR_ASSERT
#define PULSAR_ASSERT(x) if (!(x)) __debugbreak();
#endif
#ifndef PULSAR_TRY
#if PULSAR_DEBUGGING_MODE == 1
#	define PULSAR_TRY(x) PULSAR_ASSERT(glNoError(#x, __FILE__, __LINE__)) x; PULSAR_ASSERT(glNoError(#x, __FILE__, __LINE__))
#else
#	define PULSAR_TRY(x) x;
#endif
#endif // PULSAR_TRY

#include <GL/glew.h>

extern bool glNoError(const char* function_name, const char* file, int line);
