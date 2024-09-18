#include "IO.h"

#include <sstream>

bool IO::_read_file(const char* filepath, std::string& content, std::ios_base::openmode mode)
{
	std::ifstream file(filepath, mode);
	if (file)
	{
		std::ostringstream oss;
		oss << file.rdbuf();
		content = oss.str();
		return true;
	}
	return false;
}
