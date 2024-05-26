#include "AssetLoader.h"

#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <stdexcept>

#include "Utility.h"
#include "ShaderFactory.h"
#include "TextureFactory.h"

enum READ_STATUS
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
	std::string::iterator start = ++iter;
	while (*iter != ']' && iter != terminator) iter++;
	if (iter == terminator)
		return false;
	std::string::iterator end = iter++;
	header = std::string(start, end);
	return true;
}

static READ_STATUS readKeyValue(std::string::iterator& iter, const std::string::iterator& terminator, std::string& key, std::string& value)
{
	consumeWhitespace(iter);
	if (iter == terminator)
		return EMPTY;
	std::string::iterator start = iter;
	while (iter != terminator && *iter != '=') iter++;
	if (iter == terminator)
		return INCOMPLETE_KEY;
	std::string::iterator end = iter;
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
	while (iter != terminator && *iter != '#') {
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

void readFileIntoString(const char* filepath, std::string& file)
{
	std::ifstream stream(filepath);
	std::stringstream ss;
	ss << stream.rdbuf();
	// TODO use ss directly?
	file = ss.str();
}

LOAD_STATUS loadShader(const char* filepath, ShaderHandle& handle)
{
	if (!validExtension(filepath))
		return LOAD_STATUS::READ_ERR;
	
	std::string file;
	readFileIntoString(filepath, file);
	auto iter = file.begin();
	std::string header, key, value;

	if (!readHeader(iter, file.end(), header) || header != "HEADER")
		return LOAD_STATUS::SYNTAX_ERR;
	if (readKeyValue(iter, file.end(), key, value) != OK)
		return LOAD_STATUS::SYNTAX_ERR;
	if (value != "SHADER" || key != "AssetType")
		return LOAD_STATUS::MISMATCH_ERR;

	const char* vertex_shader;
	const char* fragment_shader;
	if (!readHeader(iter, file.end(), header) || header != "SHADER")
		return LOAD_STATUS::SYNTAX_ERR;
	READ_STATUS status;
	const auto kvs = readAllKeyValues(iter, file.end(), status);
	if (status == SYNTAX_ERROR)
		return LOAD_STATUS::SYNTAX_ERR;
	
	try
	{
		vertex_shader = kvs.at("vertex").c_str();
		fragment_shader = kvs.at("fragment").c_str();
	}
	catch (std::out_of_range)
	{
		return LOAD_STATUS::SYNTAX_ERR;
	}
	
	handle = ShaderFactory::GetHandle(vertex_shader, fragment_shader);
	if (handle > 0)
		return LOAD_STATUS::OK;
	else
		return LOAD_STATUS::ASSET_LOAD_ERR;
}

LOAD_STATUS loadTexture(const char* filepath, TextureHandle& handle)
{
	if (!validExtension(filepath))
		return LOAD_STATUS::READ_ERR;

	std::string file;
	readFileIntoString(filepath, file);
	auto iter = file.begin();
	std::string header, key, value;

	if (!readHeader(iter, file.end(), header) || header != "HEADER")
		return LOAD_STATUS::SYNTAX_ERR;
	if (readKeyValue(iter, file.end(), key, value) != OK)
		return LOAD_STATUS::SYNTAX_ERR;
	if (value != "TEXTURE" || key != "AssetType")
		return LOAD_STATUS::MISMATCH_ERR;

	const char* filename;
	TextureSettings settings;
	GLint lod_level(0);
	if (!readHeader(iter, file.end(), header) || header != "TEXTURE")
		return LOAD_STATUS::SYNTAX_ERR;
	READ_STATUS status;
	const auto kvs = readAllKeyValues(iter, file.end(), status);
	if (status == SYNTAX_ERROR)
		return LOAD_STATUS::SYNTAX_ERR;

	try
	{
		filename = kvs.at("path").c_str();
		if (kvs.find("lod") != kvs.end())
			lod_level = std::stoi(kvs.at("lod"));
		if (kvs.find("settings.min_filter") != kvs.end())
			settings.min_filter = static_cast<MinFilter>(std::stoi(kvs.at("settings.min_filter")));
		if (kvs.find("settings.mag_filter") != kvs.end())
			settings.mag_filter = static_cast<MagFilter>(std::stoi(kvs.at("settings.mag_filter")));
		if (kvs.find("settings.wrap_s") != kvs.end())
			settings.wrap_s = static_cast<TextureWrap>(std::stoi(kvs.at("settings.wrap_s")));
		if (kvs.find("settings.wrap_t") != kvs.end())
			settings.wrap_t = static_cast<TextureWrap>(std::stoi(kvs.at("settings.wrap_t")));
	}
	catch (std::out_of_range)
	{
		return LOAD_STATUS::SYNTAX_ERR;
	}
	
	handle = TextureFactory::GetHandle(filename, settings, lod_level);
	if (handle > 0)
		return LOAD_STATUS::OK;
	else
		return LOAD_STATUS::ASSET_LOAD_ERR;
}
