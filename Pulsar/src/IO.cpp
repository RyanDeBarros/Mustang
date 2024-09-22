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

static const char* ios_mode_to_c_mode(std::ios_base::openmode mode)
{
    if (mode & std::ios_base::binary)
        if (mode & std::ios_base::in && mode & std::ios_base::out)
            return "r+b";
        else if (mode & std::ios_base::in)
            return "rb";
        else if (mode & std::ios_base::out)
            return "wb";
    else
        if (mode & std::ios_base::in && mode & std::ios_base::out)
            return "r+";
        else if (mode & std::ios_base::in)
            return "r";
        else if (mode & std::ios_base::out)
            return "w";
    return ""; // return empty string if the mode is unsupported
}

bool IO::_read_file_uc(const char* filepath, unsigned char*& content, size_t& content_length, std::ios_base::openmode mode)
{
    FILE* file;
    if (fopen_s(&file, filepath, ios_mode_to_c_mode(mode)) == 0 && file)
    {
        fseek(file, 0, SEEK_END);
        content_length = ftell(file);
        fseek(file, 0, SEEK_SET);
        content = new unsigned char[content_length];
        fread(content, content_length, 1, file);
        fclose(file);
        return true;
    }
    return false;
}
