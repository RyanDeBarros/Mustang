#include "AssetLoader.h"

#include "Utility.h"

#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <stdexcept>

static enum READ_STATUS
{
	OK,
	EMPTY,
	INCOMPLETE_KEY,
	SYNTAX_ERROR
};

static bool validExtension(const char* filepath)
{
	const char* dot = strrchr(filepath, '.');
	if (!dot || dot == filepath) return false;
	dot++;
	return strlen(dot) == 4 && tolower(dot[0]) == 'm' && tolower(dot[1]) == 'a' && tolower(dot[2]) == 's' && tolower(dot[3]) == 's';
}

static void consumeWhitespace(std::string::iterator& iter)
{
	while (isspace(*iter)) iter++;
}

static bool readHeader(std::string::iterator& iter, const std::string::iterator& terminator, std::string& header)
{
	consumeWhitespace(iter);
	if (iter == terminator || *iter != '[')
		return false;
	auto start = ++iter;
	while (*iter != ']' && iter != terminator) iter++;
	if (iter == terminator)
		return false;
	auto end = iter++;
	header = std::string(start, end);
	return true;
}

static READ_STATUS readKeyValue(std::string::iterator& iter, const std::string::iterator& terminator, std::string& key, std::string& value)
{
	consumeWhitespace(iter);
	if (iter == terminator)
		return EMPTY;
	auto start = iter;
	while (*iter != '=' && iter != terminator) iter++;
	if (iter == terminator)
		return INCOMPLETE_KEY;
	auto end = iter;
	key = std::string(start, end);
	start = ++iter;
	while (*iter != '\n' && *iter != '\r' && iter != terminator) iter++;
	end = iter;
	value = std::string(start, end);
	return OK;
}

static std::unordered_map<std::string, std::string> readAllKeyValues(std::string::iterator& iter, const std::string::iterator& terminator, READ_STATUS& status)
{
	consumeWhitespace(iter);
	std::unordered_map<std::string, std::string> kvs;
	std::string key, value;
	while (*iter != '#' && iter != terminator) {
		READ_STATUS status = readKeyValue(iter, terminator, key, value);
		if (status == OK)
			kvs[key] = value;
		else if (status != EMPTY)
		{
			status = SYNTAX_ERROR;
			return kvs;
		}
		consumeWhitespace(iter);
	}
	if (iter != terminator)
		iter++;
	status = OK;
	return kvs;
}

LOAD_STATUS loadShader(const char* filepath, ShaderHandle& handle)
{
	if (!validExtension(filepath))
		return LOAD_STATUS::READ_ERR;

	std::ifstream stream(filepath);
	std::stringstream ss;
	ss << stream.rdbuf();
	// TODO use ss directly?
	std::string file = ss.str();
	
	const char* vertex_shader;
	const char* fragment_shader;

	auto iter = file.begin();
	std::string header, key, value;

	if (!readHeader(iter, file.end(), header) || header != "HEADER")
		return LOAD_STATUS::SYNTAX_ERR;
	if (readKeyValue(iter, file.end(), key, value) != OK)
		return LOAD_STATUS::SYNTAX_ERR;
	if (value != std::string_view("SHADER"))
		return LOAD_STATUS::MISMATCH_ERR;

	if (!readHeader(iter, file.end(), header) || header != "SHADER")
		return LOAD_STATUS::SYNTAX_ERR;
	READ_STATUS status;
	const auto kvs = readAllKeyValues(iter, file.end(), status);
	if (status == SYNTAX_ERROR)
		return LOAD_STATUS::SYNTAX_ERR;
	try
	{
		std::string ref = kvs.at("ref");
		// TODO load referenced shader
		return LOAD_STATUS::SYNTAX_ERR;
	}
	catch (std::out_of_range)
	{
		try
		{
			vertex_shader = kvs.at("vertex").c_str();
			fragment_shader = kvs.at("fragment").c_str();
		}
		catch (std::out_of_range)
		{
			return LOAD_STATUS::SYNTAX_ERR;
		}
	}
	
	handle = ShaderFactory::GetHandle(vertex_shader, fragment_shader);
	return LOAD_STATUS::OK;
}
