#include "Logger.h"

#include <iostream>

#include "Utility.h"

namespace Logger {

	void LogInfo(const char* message)
	{
		std::cout << message << std::endl;
	}

	void LogInfo(const unsigned char* message)
	{
		std::cout << message << std::endl;
	}

	void LogInfo(const std::string& message)
	{
		std::cout << message << std::endl;
	}
	
	void LogInfo(const unsigned int message)
	{
		std::cout << message << std::endl;
	}

	void LogWarning(const char* message, const char* source)
	{
		std::cout << "[Warning] " << message << "\tSource (" << source << " " << __FILE__ << ":" << __LINE__ << ")" << std::endl;
	}

	void LogWarning(const unsigned char* message, const char* source)
	{
		std::cout << "[Warning] " << message << "\tSource (" << source << " " << __FILE__ << ":" << __LINE__ << ")" << std::endl;
	}

	void LogWarning(const std::string& message, const char* source)
	{
		std::cout << "[Warning] " << message << "\tSource (" << source << " " << __FILE__ << ":" << __LINE__ << ")" << std::endl;
	}

	void LogWarning(const unsigned int message, const char* source)
	{
		std::cout << "[Warning] " << message << "\tSource (" << source << " " << __FILE__ << ":" << __LINE__ << ")" << std::endl;
	}

	void LogError(const char* message, const char* source)
	{
		std::cerr << "[Error] " << message << "\tSource (" << source << " " << __FILE__ << ":" << __LINE__ << ")" << std::endl;
	}

	void LogError(const unsigned char* message, const char* source)
	{
		std::cerr << "[Error] " << message << "\tSource (" << source << " " << __FILE__ << ":" << __LINE__ << ")" << std::endl;
	}
	
	void LogError(const std::string& message, const char* source)
	{
		std::cerr << "[Error] " << message << "\tSource (" << source << " " << __FILE__ << ":" << __LINE__ << ")" << std::endl;
	}

	void LogError(const unsigned int message, const char* source)
	{
		std::cerr << "[Error] " << message << "\tSource (" << source << " " << __FILE__ << ":" << __LINE__ << ")" << std::endl;
	}

	void LogErrorFatal(const char* message, const char* source)
	{
		std::cerr << "[Fatal] " << message << "\tSource (" << source << " " << __FILE__ << ":" << __LINE__ << ")" << std::endl;
		ASSERT(false);
	}

	void LogErrorFatal(const unsigned char* message, const char* source)
	{
		std::cerr << "[Fatal] " << message << "\tSource (" << source << " " << __FILE__ << ":" << __LINE__ << ")" << std::endl;
		ASSERT(false);
	}

	void LogErrorFatal(const std::string& message, const char* source)
	{
		std::cerr << "[Fatal] " << message << "\tSource (" << source << " " << __FILE__ << ":" << __LINE__ << ")" << std::endl;
		ASSERT(false);
	}

	void LogErrorFatal(const unsigned int message, const char* source)
	{
		std::cerr << "[Fatal] " << message << "\tSource (" << source << " " << __FILE__ << ":" << __LINE__ << ")" << std::endl;
		ASSERT(false);
	}

}
