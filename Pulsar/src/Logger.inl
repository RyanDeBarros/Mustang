#pragma once

#include <iostream>

// eventually, Pulsar namespace will be used for everything. maybe then, an additional Logger namespace won't be necessary.

namespace Logger {
	
	inline void Wait()
	{
		std::cin.get();
	}

	inline void NewLine()
	{
		std::cout << std::endl;
	}

	inline void LogInfo(const auto& message)
	{
		std::cout << message << std::endl;
	}

	inline void LogWarning(const auto& message, const char* source = "", const char* file = "", const char* line = "")
	{
		if (source[0] != '\0')
			std::cout << "[Warning] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Warning] " << message << std::endl;
	}

	inline void LogError(const auto& message, const char* source = "", const char* file = "", const char* line = "")
	{
		if (source[0] != '\0')
			std::cout << "[Error] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Error] " << message << std::endl;
	}

	inline void LogErrorFatal(const auto& message, const char* source = "", const char* file = "", const char* line = "")
	{
		if (source[0] != '\0')
			std::cout << "[Fatal] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Fatal] " << message << std::endl;
		__debugbreak();
	}

}
