#pragma once

#include <fstream>
#include <string>

class IO
{
	IO() = default;
	IO(const IO&) = delete;
	IO(IO&&) = delete;
	~IO() = default;

	static IO io;

	bool _read_file(const char* filepath, std::string& content, std::ios_base::openmode mode = std::ios_base::in);
	bool _read_file_uc(const char* filepath, unsigned char*& content, size_t& content_length,
		std::ios_base::openmode mode = std::ios_base::in | std::ios_base::binary);

public:
	static bool read_file(const char* filepath, std::string& content, std::ios_base::openmode mode = std::ios_base::in)
	{
		return io._read_file(filepath, content, mode);
	}
	static bool read_file_uc(const char* filepath, unsigned char*& content, size_t& content_length,
		std::ios_base::openmode mode = std::ios_base::in | std::ios_base::binary)
	{
		return io._read_file_uc(filepath, content, content_length, mode);
	}
};

inline IO IO::io;
