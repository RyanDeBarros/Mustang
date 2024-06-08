#pragma once

#include <string>
#include <format>


namespace Logger {
	
	void Wait();
	void LogInfo(const char* message);
	void LogInfo(const unsigned char* message);
	void LogInfo(const std::string& message);
	void LogInfo(const unsigned int message);
	void LogInfo(const int message);
	void LogInfo(const double message);
	void LogWarning(const char* message, const char* source="", const char* file="", const char* line="");
	void LogWarning(const unsigned char* message, const char* source="", const char* file="", const char* line="");
	void LogWarning(const std::string& message, const char* source="", const char* file="", const char* line="");
	void LogWarning(const unsigned int message, const char* source="", const char* file="", const char* line="");
	void LogError(const char* message, const char* source="", const char* file="", const char* line="");
	void LogError(const unsigned char* message, const char* source="", const char* file="", const char* line="");
	void LogError(const std::string& message, const char* source="", const char* file="", const char* line="");
	void LogError(const unsigned int message, const char* source="", const char* file="", const char* line="");
	void LogErrorFatal(const char* message, const char* source="", const char* file="", const char* line="");
	void LogErrorFatal(const unsigned char* message, const char* source="", const char* file="", const char* line="");
	void LogErrorFatal(const std::string& message, const char* source="", const char* file="", const char* line="");
	void LogErrorFatal(const unsigned int message, const char* source="", const char* file="", const char* line="");

}
