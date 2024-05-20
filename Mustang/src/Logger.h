#pragma once

#include <string>

namespace Logger {
	
	void LogInfo(const char* message);
	void LogInfo(const unsigned char* message);
	void LogInfo(const std::string& message);
	void LogInfo(const unsigned int message);
	void LogWarning(const char* message, const char* source="");
	void LogWarning(const unsigned char* message, const char* source="");
	void LogWarning(const std::string& message, const char* source="");
	void LogWarning(const unsigned int message, const char* source="");
	void LogError(const char* message, const char* source="");
	void LogError(const unsigned char* message, const char* source="");
	void LogError(const std::string& message, const char* source="");
	void LogError(const unsigned int message, const char* source="");
	void LogErrorFatal(const char* message, const char* source="");
	void LogErrorFatal(const unsigned char* message, const char* source="");
	void LogErrorFatal(const std::string& message, const char* source="");
	void LogErrorFatal(const unsigned int message, const char* source="");

}
