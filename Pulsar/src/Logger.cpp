#include "Logger.h"

#include <iostream>

#include "Macros.h"

namespace Logger {

	void Wait()
	{
		std::cin.get();
	}

	void NewLine()
	{
		std::cout << std::endl;
	}

	void LogInfo(const char* message)
	{
		std::cout << message << std::endl;
	}

	void LogInfo(const std::string& message)
	{
		std::cout << message << std::endl;
	}
	
	void LogInfo(unsigned int message)
	{
		std::cout << message << std::endl;
	}

	void LogInfo(int message)
	{
		std::cout << message << std::endl;
	}

	void LogInfo(double message)
	{
		std::cout << message << std::endl;
	}

	void LogWarning(const char* message, const char* source, const char* file, const char* line)
	{
		if (source[0] != '\0')
			std::cout << "[Warning] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Warning] " << message << std::endl;
	}

	void LogWarning(const std::string& message, const char* source, const char* file, const char* line)
	{
		if (source[0] != '\0')
			std::cout << "[Warning] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Warning] " << message << std::endl;
	}

	void LogWarning(unsigned int message, const char* source, const char* file, const char* line)
	{
		if (source[0] != '\0')
			std::cout << "[Warning] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Warning] " << message << std::endl;
	}

	void LogError(const char* message, const char* source, const char* file, const char* line)
	{
		if (source[0] != '\0')
			std::cout << "[Error] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Error] " << message << std::endl;
	}
	
	void LogError(const std::string& message, const char* source, const char* file, const char* line)
	{
		if (source[0] != '\0')
			std::cout << "[Error] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Error] " << message << std::endl;
	}

	void LogError(unsigned int message, const char* source, const char* file, const char* line)
	{
		if (source[0] != '\0')
			std::cout << "[Error] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Error] " << message << std::endl;
	}

	void LogErrorFatal(const char* message, const char* source, const char* file, const char* line)
	{
		if (source[0] != '\0')
			std::cout << "[Fatal] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Fatal] " << message << std::endl;
		ASSERT(false);
	}

	void LogErrorFatal(const std::string& message, const char* source, const char* file, const char* line)
	{
		if (source[0] != '\0')
			std::cout << "[Fatal] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Fatal] " << message << std::endl;
		ASSERT(false);
	}

	void LogErrorFatal(unsigned int message, const char* source, const char* file, const char* line)
	{
		if (source[0] != '\0')
			std::cout << "[Fatal] " << message << "\tSource (" << source << " " << file << ":" << line << ")" << std::endl;
		else
			std::cout << "[Fatal] " << message << std::endl;
		ASSERT(false);
	}

}
