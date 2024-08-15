#include "Macros.h"

#include <string>

#include "Logger.inl"

bool glNoError(const char* function_name, const char* file, int line)
{
	bool no_err = true;
	while (GLenum error = glGetError())
	{
		Logger::LogError(error, function_name, file, std::to_string(line).c_str());
		no_err = false;
	}
	return no_err;
}
