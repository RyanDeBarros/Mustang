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

	template<typename T>
	inline void LogInfo(const T& message)
	{
		std::cout << message << std::endl;
	}

	template<typename T>
	inline void LogWarning(const T& message, const char* source = "", const char* file = "", const char* line = "")
	{
		if (source[0] != '\0')
			std::cout << "[Warning] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Warning] " << message << std::endl;
	}

	template<typename T>
	inline void LogError(const T& message, const char* source = "", const char* file = "", const char* line = "")
	{
		if (source[0] != '\0')
			std::cout << "[Error] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Error] " << message << std::endl;
	}

	template<typename T>
	inline void LogErrorFatal(const T& message, const char* source = "", const char* file = "", const char* line = "")
	{
		if (source[0] != '\0')
			std::cout << "[Fatal] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Fatal] " << message << std::endl;
		__debugbreak();
	}

}
