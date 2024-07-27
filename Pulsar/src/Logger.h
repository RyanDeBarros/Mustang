#pragma once

#include <string>
#include <format>

// eventually, Pulsar namespace will be used for everything. maybe then, an additional Logger namespace won't be necessary.

namespace Logger {
	
	void Wait();
	void NewLine();
	void LogInfo(const char* message);
	void LogInfo(const std::string& message);
	void LogInfo(unsigned int message);
	void LogInfo(int message);
	void LogInfo(double message);
	void LogWarning(const char* message, const char* source="", const char* file="", const char* line="");
	void LogWarning(const std::string& message, const char* source="", const char* file="", const char* line="");
	void LogWarning(unsigned int message, const char* source="", const char* file="", const char* line="");
	void LogError(const char* message, const char* source="", const char* file="", const char* line="");
	void LogError(const std::string& message, const char* source="", const char* file="", const char* line="");
	void LogError(unsigned int message, const char* source="", const char* file="", const char* line="");
	void LogErrorFatal(const char* message, const char* source="", const char* file="", const char* line="");
	void LogErrorFatal(const std::string& message, const char* source="", const char* file="", const char* line="");
	void LogErrorFatal(unsigned int message, const char* source="", const char* file="", const char* line="");

}
