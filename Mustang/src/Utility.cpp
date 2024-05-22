#include "Utility.h"

#include <stdio.h>

namespace Utility {

	char* ReadFile(const char* filepath)
	{
		FILE* file;
		fopen_s(&file, filepath, "rb");
		if (file)
		{
			fseek(file, 0, SEEK_END);
			const long filesize = ftell(file);
			fseek(file, 0, SEEK_SET);

			char* buffer = new char[filesize + 1];
			fread(buffer, 1, filesize + 1, file);
			buffer[filesize] = '\0';
			return buffer;
		}
		else
		{
			Logger::LogError(std::string("Could not read file \"") + filepath + "\"");
			return nullptr;
		}
	}

}
