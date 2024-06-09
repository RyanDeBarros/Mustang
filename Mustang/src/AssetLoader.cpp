#include "AssetLoader.h"

#include <toml/tomlcpp.hpp>

#include "Utility.h"
#include "ShaderFactory.h"
#include "TextureFactory.h"

static bool validExtension(const char* filepath)
{
	const char* dot = strrchr(filepath, '.');
	if (!dot || dot == filepath) return false;
	dot++;
	bool valid = (strlen(dot) == 4 && tolower(dot[0]) == 'm' && tolower(dot[1]) == 'a' && tolower(dot[2]) == 's' && tolower(dot[3]) == 's')
			|| (strlen(dot) == 4 && tolower(dot[0]) == 't' && tolower(dot[1]) == 'o' && tolower(dot[2]) == 'm' && tolower(dot[3]) == 'l');
	if (!valid)
		Logger::LogError("Asset file has incorrect file extension: " + std::string(filepath));
	return valid;
}

LOAD_STATUS loadShader(const char* filepath, ShaderHandle& handle)
{
	if (!validExtension(filepath))
		return LOAD_STATUS::READ_ERR;
	auto res = toml::parseFile(filepath);
	if (!res.table)
	{
		Logger::LogError("Cannot parse asset file: " + res.errmsg);
		return LOAD_STATUS::READ_ERR;
	}

	auto [ok0, header] = res.table->getString("header");
	if (!ok0)
		return LOAD_STATUS::SYNTAX_ERR;
	else if (header != "shader")
		return LOAD_STATUS::MISMATCH_ERR;

	auto shader = res.table->getTable("shader");
	if (!shader)
		return LOAD_STATUS::SYNTAX_ERR;

	auto [ok1, vertex_shader] = shader->getString("vertex");
	if (!ok1)
		return LOAD_STATUS::SYNTAX_ERR;
	auto [ok2, fragment_shader] = shader->getString("fragment");
	if (!ok2)
		return LOAD_STATUS::SYNTAX_ERR;
	
	handle = ShaderFactory::GetHandle(vertex_shader.c_str(), fragment_shader.c_str());
	if (handle > 0)
		return LOAD_STATUS::OK;
	else
		return LOAD_STATUS::ASSET_LOAD_ERR;
}

LOAD_STATUS loadTexture(const char* filepath, TextureHandle& handle)
{
	if (!validExtension(filepath))
		return LOAD_STATUS::READ_ERR;
	auto res = toml::parseFile(filepath);
	if (!res.table)
	{
		Logger::LogError("Cannot parse asset file: " + res.errmsg);
		return LOAD_STATUS::READ_ERR;
	}

	auto [ok0, header] = res.table->getString("header");
	if (!ok0)
		return LOAD_STATUS::SYNTAX_ERR;
	else if (header != "texture")
		return LOAD_STATUS::MISMATCH_ERR;

	auto texture = res.table->getTable("texture");
	if (!texture)
		return LOAD_STATUS::SYNTAX_ERR;

	auto [ok1, path] = texture->getString("path");
	if (!ok1)
		return LOAD_STATUS::SYNTAX_ERR;

	TextureSettings texture_settings;
	auto settings = texture->getTable("settings");
	if (settings)
	{
		auto [ok2, min_filter] = settings->getInt("min_filter");
		if (ok2)
		{
			if (min_filter < 0 || min_filter >= MinFilterLookupLength)
				return LOAD_STATUS::SYNTAX_ERR;
			texture_settings.min_filter = MinFilterLookup[min_filter];
		}
		auto [ok3, mag_filter] = settings->getInt("mag_filter");
		if (ok3)
		{
			if (mag_filter < 0 || mag_filter >= MagFilterLookupLength)
				return LOAD_STATUS::SYNTAX_ERR;
			texture_settings.mag_filter = MagFilterLookup[mag_filter];
		}
		auto [ok4, wrap_s] = settings->getInt("wrap_s");
		if (ok4)
		{
			if (wrap_s < 0 || wrap_s >= TextureWrapLookupLength)
				return LOAD_STATUS::SYNTAX_ERR;
			texture_settings.wrap_s = TextureWrapLookup[wrap_s];
		}
		auto [ok5, wrap_t] = settings->getInt("wrap_t");
		if (ok5)
		{
			if (wrap_t < 0 || wrap_t >= TextureWrapLookupLength)
				return LOAD_STATUS::SYNTAX_ERR;
			texture_settings.wrap_t = TextureWrapLookup[wrap_t];
		}
		auto [ok6, lod_level] = settings->getInt("lod_level");
		if (ok6)
		{
			texture_settings.lod_level = lod_level;
		}
	}

	handle = TextureFactory::GetHandle(path.c_str(), texture_settings);
	if (handle > 0)
		return LOAD_STATUS::OK;
	else
		return LOAD_STATUS::ASSET_LOAD_ERR;
}
