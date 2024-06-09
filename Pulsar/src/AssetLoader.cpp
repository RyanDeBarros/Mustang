#include "AssetLoader.h"

#include <toml/tomlcpp.hpp>

#include "Logger.h"
#include "RendererSettings.h"
#include "factory/ShaderFactory.h"
#include "factory/TextureFactory.h"
#include "render/Renderable.h"

LOAD_STATUS loadShader(const char* filepath, ShaderHandle& handle)
{
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

LOAD_STATUS loadRenderable(const char* filepath, Renderable& renderable)
{
	auto res = toml::parseFile(filepath);
	if (!res.table)
	{
		Logger::LogError("Cannot parse asset file: " + res.errmsg);
		return LOAD_STATUS::READ_ERR;
	}

	auto [ok0, header] = res.table->getString("header");
	if (!ok0)
		return LOAD_STATUS::SYNTAX_ERR;
	else if (header != "renderable")
		return LOAD_STATUS::MISMATCH_ERR;

	auto _renderable = res.table->getTable("renderable");
	if (!_renderable)
		return LOAD_STATUS::SYNTAX_ERR;

	TextureHandle texture_handle = 0;
	auto [ok2, texture] = _renderable->getString("texture");
	if (ok2)
	{
		if (loadTexture(texture.c_str(), texture_handle) != LOAD_STATUS::OK)
			return LOAD_STATUS::REFERENCE_ERROR;
	}
	renderable.textureHandle = texture_handle;

	auto model = _renderable->getTable("model");
	if (!model)
		return LOAD_STATUS::SYNTAX_ERR;
	auto [ok3, layout] = model->getInt("layout");
	if (!ok3)
		return LOAD_STATUS::SYNTAX_ERR;
	auto [ok4, mask] = model->getInt("mask");
	if (!ok4)
		return LOAD_STATUS::SYNTAX_ERR;
	renderable.model.layout = layout;
	renderable.model.layoutMask = mask;
	
	ShaderHandle shader_handle = 0;
	auto [ok5, shader] = model->getString("shader");
	if (ok5)
	{
		if (loadShader(shader.c_str(), shader_handle) != LOAD_STATUS::OK)
			return LOAD_STATUS::REFERENCE_ERROR;
	}
	renderable.model.shader = shader_handle;

	auto vertex_array = _renderable->getArray("vertices");
	if (!vertex_array)
		return LOAD_STATUS::SYNTAX_ERR;
	auto [ok1, num_vertices] = _renderable->getInt("num_vertices");
	if (!ok1)
		return LOAD_STATUS::SYNTAX_ERR;
	renderable.vertexCount = num_vertices;
	if (!renderable.AttachVertexBuffer(*vertex_array, Render::VertexBufferLayoutCount(renderable)))
		return LOAD_STATUS::ASSET_LOAD_ERR;

	auto index_array = _renderable->getArray("indices");
	if (!index_array)
		return LOAD_STATUS::SYNTAX_ERR;
	renderable.indexCount = index_array->size();
	if (!renderable.AttachIndexBuffer(*index_array, renderable.indexCount))
		return LOAD_STATUS::ASSET_LOAD_ERR;

	return LOAD_STATUS::OK;
}

bool _LoadRendererSettings()
{
	auto res = toml::parseFile(_RendererSettings::settings_filepath);
	if (!res.table)
	{
		Logger::LogErrorFatal("Cannot parse renderer settings file: " + res.errmsg);
		return false;
	}

	auto window = res.table->getTable("window");
	if (window)
	{
		auto [ok0, width] = window->getInt("width");
		if (ok0)
			_RendererSettings::window_width = width;
		auto [ok1, height] = window->getInt("height");
		if (ok1)
			_RendererSettings::window_height = height;
	}

	auto rendering = res.table->getTable("rendering");
	if (rendering)
	{
		auto [ok0, max_texture_slots] = rendering->getInt("max_texture_slots");
		if (ok0)
			_RendererSettings::max_texture_slots = max_texture_slots;
	}

	return true;
}
