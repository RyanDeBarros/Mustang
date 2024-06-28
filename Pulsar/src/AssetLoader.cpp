#include "AssetLoader.h"

#include <toml/tomlcpp.hpp>
#include <glm/glm.hpp>
#include <stb/stb_image_write.h>

#include <map>

#include "Logger.h"
#include "RendererSettings.h"
#include "factory/ShaderFactory.h"
#include "factory/TextureFactory.h"
#include "factory/UniformLexiconFactory.h"
#include "factory/UniformLexicon.h"
#include "factory/Atlas.h"
#include "render/Renderable.h"

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
			_RendererSettings::initial_window_width = (int)width;
		auto [ok1, height] = window->getInt("height");
		if (ok1)
			_RendererSettings::initial_window_height = (int)height;
		auto gl_clear_color = window->getArray("gl_clear_color");
		if (gl_clear_color)
		{
			auto color = gl_clear_color->getDoubleVector();
			if (color)
				_RendererSettings::gl_clear_color = glm::vec4((*color)[0], (*color)[1], (*color)[2], (*color)[3]);
		}
	}

	auto rendering = res.table->getTable("rendering");
	if (rendering)
	{
		auto [ok0, max_texture_slots] = rendering->getInt("max_texture_slots");
		if (ok0)
			_RendererSettings::max_texture_slots = (TextureSlot)max_texture_slots;
		auto [ok1, standard_vertex_pool_size] = rendering->getInt("standard_vertex_pool_size");
		if (ok1)
			_RendererSettings::standard_vertex_pool_size = (VertexSize)standard_vertex_pool_size;
		auto [ok2, standard_index_pool_size] = rendering->getInt("standard_index_pool_size");
		if (ok2)
			_RendererSettings::standard_index_pool_size = (VertexSize)standard_index_pool_size;
		auto [ok3, standard_shader_filepath] = rendering->getString("standard_shader");
		if (ok3)
			_RendererSettings::standard_shader_assetfile = standard_shader_filepath;
		auto [ok4, rect_renderable] = rendering->getString("rect_renderable");
		if (ok4)
			_RendererSettings::rect_renderable_filepath = rect_renderable;
	}

	return true;
}

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

LOAD_STATUS loadTexture(const char* filepath, TextureHandle& handle, const bool& new_texture, const bool& temporary_buffer)
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
			texture_settings.lod_level = static_cast<GLint>(lod_level);
		}
	}

	handle = TextureFactory::GetHandle(path.c_str(), texture_settings, new_texture, temporary_buffer);
	if (handle > 0)
		return LOAD_STATUS::OK;
	else
		return LOAD_STATUS::ASSET_LOAD_ERR;
}

LOAD_STATUS loadUniformLexicon(const char* filepath, UniformLexiconHandle& handle)
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
	else if (header != "uniform_lexicon")
		return LOAD_STATUS::MISMATCH_ERR;

	auto array = res.table->getArray("uniform");
	if (!array)
		return LOAD_STATUS::SYNTAX_ERR;
	std::map<std::string, Uniform> uniform_map;
	auto uniforms = array->getTableVector();
	for (const auto& uniform : *uniforms)
	{
		auto [ok1, type] = uniform.getInt("type");
		auto [ok2, name] = uniform.getString("name");
		Uniform u;
		if (!ok1 || !ok2)
			return LOAD_STATUS::SYNTAX_ERR;
		switch (type)
		{
		case 0:
		{
			auto a = uniform.getInt("value");
			if (!a.first)
				return LOAD_STATUS::SYNTAX_ERR;
			u = static_cast<GLint>(a.second);
			break;
		}
		case 1:
		{
			auto a = uniform.getArray("value");
			if (!a)
				return LOAD_STATUS::SYNTAX_ERR;
			u = glm::ivec2(a->getInt(0).second, a->getInt(1).second);
			break;
		}
		case 2:
		{
			auto a = uniform.getArray("value");
			if (!a)
				return LOAD_STATUS::SYNTAX_ERR;
			u = glm::ivec3(a->getInt(0).second, a->getInt(1).second, a->getInt(2).second);
			break;
		}
		case 3:
		{
			auto a = uniform.getArray("value");
			if (!a)
				return LOAD_STATUS::SYNTAX_ERR;
			u = glm::ivec4(a->getInt(0).second, a->getInt(1).second, a->getInt(2).second, a->getInt(3).second);
			break;
		}
		case 4:
		{
			auto a = uniform.getInt("value");
			if (!a.first)
				return LOAD_STATUS::SYNTAX_ERR;
			u = static_cast<GLint>(a.second);
			break;
		}
		case 5:
		{
			auto a = uniform.getArray("value");
			if (!a)
				return LOAD_STATUS::SYNTAX_ERR;
			u = glm::uvec2(a->getInt(0).second, a->getInt(1).second);
			break;
		}
		case 6:
		{
			auto a = uniform.getArray("value");
			if (!a)
				return LOAD_STATUS::SYNTAX_ERR;
			u = glm::uvec3(a->getInt(0).second, a->getInt(1).second, a->getInt(2).second);
			break;
		}
		case 7:
		{
			auto a = uniform.getArray("value");
			if (!a)
				return LOAD_STATUS::SYNTAX_ERR;
			u = glm::uvec4(a->getInt(0).second, a->getInt(1).second, a->getInt(2).second, a->getInt(3).second);
			break;
		}
		case 8:
		{
			auto a = uniform.getDouble("value");
			if (!a.first)
				return LOAD_STATUS::SYNTAX_ERR;
			u = static_cast<GLint>(a.second);
			break;
		}
		case 9:
		{
			auto a = uniform.getArray("value");
			if (!a)
				return LOAD_STATUS::SYNTAX_ERR;
			u = glm::vec2(static_cast<float>(a->getDouble(0).second), static_cast<float>(a->getDouble(1).second));
			break;
		}
		case 10:
		{
			auto a = uniform.getArray("value");
			if (!a)
				return LOAD_STATUS::SYNTAX_ERR;
			u = glm::vec3(static_cast<float>(a->getDouble(0).second), static_cast<float>(a->getDouble(1).second), static_cast<float>(a->getDouble(2).second));
			break;
		}
		case 11:
		{
			auto a = uniform.getArray("value");
			if (!a)
				return LOAD_STATUS::SYNTAX_ERR;
			u = glm::vec4(static_cast<float>(a->getDouble(0).second), static_cast<float>(a->getDouble(1).second), static_cast<float>(a->getDouble(2).second), static_cast<float>(a->getDouble(3).second));
			break;
		}
		case 12:
		{
			auto a = uniform.getArray("value");
			if (!a)
				return LOAD_STATUS::SYNTAX_ERR;
			auto col0 = a->getArray(0);
			auto col1 = a->getArray(1);
			if (!col0 || !col1)
				return LOAD_STATUS::SYNTAX_ERR;
			u = glm::mat2(static_cast<float>(col0->getDouble(0).second), static_cast<float>(col0->getDouble(1).second), static_cast<float>(col1->getDouble(0).second), static_cast<float>(col1->getDouble(1).second));
			break;
		}
		case 13:
		{
			auto a = uniform.getArray("value");
			if (!a)
				return LOAD_STATUS::SYNTAX_ERR;
			auto col0 = a->getArray(0);
			auto col1 = a->getArray(1);
			auto col2 = a->getArray(2);
			if (!col0 || !col1 || !col2)
				return LOAD_STATUS::SYNTAX_ERR;
			u = glm::mat3(
				static_cast<float>(col0->getDouble(0).second), static_cast<float>(col0->getDouble(1).second), static_cast<float>(col0->getDouble(2).second),
				static_cast<float>(col1->getDouble(0).second), static_cast<float>(col1->getDouble(1).second), static_cast<float>(col1->getDouble(2).second),
				static_cast<float>(col2->getDouble(0).second), static_cast<float>(col2->getDouble(1).second), static_cast<float>(col2->getDouble(2).second)
			);
			break;
		}
		case 14:
		{
			auto a = uniform.getArray("value");
			if (!a)
				return LOAD_STATUS::SYNTAX_ERR;
			auto col0 = a->getArray(0);
			auto col1 = a->getArray(1);
			auto col2 = a->getArray(2);
			auto col3 = a->getArray(3);
			if (!col0 || !col1 || !col2 || !col3)
				return LOAD_STATUS::SYNTAX_ERR;
			u = glm::mat4(
				static_cast<float>(col0->getDouble(0).second), static_cast<float>(col0->getDouble(1).second), static_cast<float>(col0->getDouble(2).second), static_cast<float>(col0->getDouble(3).second),
				static_cast<float>(col1->getDouble(0).second), static_cast<float>(col1->getDouble(1).second), static_cast<float>(col1->getDouble(2).second), static_cast<float>(col1->getDouble(3).second),
				static_cast<float>(col2->getDouble(0).second), static_cast<float>(col2->getDouble(1).second), static_cast<float>(col2->getDouble(2).second), static_cast<float>(col2->getDouble(3).second),
				static_cast<float>(col3->getDouble(0).second), static_cast<float>(col3->getDouble(1).second), static_cast<float>(col3->getDouble(2).second), static_cast<float>(col3->getDouble(3).second)
			);
			break;
		}
		}
		uniform_map.insert({ name, u });
	}
	handle = UniformLexiconFactory::GetHandle(uniform_map);
	return LOAD_STATUS::OK;
}

LOAD_STATUS loadRenderable(const char* filepath, Renderable& renderable, const bool& new_texture, const bool& temporary_buffer)
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
	auto [ok1, texture] = _renderable->getString("texture");
	if (ok1)
	{
		if (loadTexture(texture.c_str(), texture_handle, new_texture, temporary_buffer) != LOAD_STATUS::OK)
			return LOAD_STATUS::REFERENCE_ERROR;
	}
	renderable.textureHandle = texture_handle;

	auto model = _renderable->getTable("model");
	if (!model)
		return LOAD_STATUS::SYNTAX_ERR;
	auto [ok2, layout] = model->getInt("layout");
	if (!ok2)
		return LOAD_STATUS::SYNTAX_ERR;
	auto [ok3, mask] = model->getInt("mask");
	if (!ok3)
		return LOAD_STATUS::SYNTAX_ERR;
	renderable.model.layout = static_cast<VertexLayout>(layout);
	renderable.model.layoutMask = static_cast<VertexLayoutMask>(mask);

	ShaderHandle shader_handle = 0;
	auto [ok4, shader] = model->getString("shader");
	if (ok4)
	{
		if (loadShader(shader.c_str(), shader_handle) != LOAD_STATUS::OK)
			return LOAD_STATUS::REFERENCE_ERROR;
	}
	renderable.model.shader = shader_handle;

	UniformLexiconHandle lexicon_handle = 0;
	auto [ok5, uniformLexicon] = model->getString("uniform_lexicon");
	if (ok5)
	{
		if (loadUniformLexicon(uniformLexicon.c_str(), lexicon_handle) != LOAD_STATUS::OK)
			return LOAD_STATUS::REFERENCE_ERROR;
	}
	renderable.model.uniformLexicon = lexicon_handle;

	auto vertex_array = _renderable->getArray("vertices");
	if (!vertex_array)
		return LOAD_STATUS::SYNTAX_ERR;
	auto [ok6, num_vertices] = _renderable->getInt("num_vertices");
	if (!ok6)
		return LOAD_STATUS::SYNTAX_ERR;
	renderable.vertexCount = static_cast<BufferCounter>(num_vertices);
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

bool saveAtlas(const Atlas& atlas, const char* texture_filepath, const char* asset_filepath, const char* image_format, unsigned char jpg_quality)
{
	// TODO create asset file that encapsulates the subtextures of the atlas.
	
	if (strcmp(image_format, "png"))
		stbi_write_png(texture_filepath, atlas.GetWidth(), atlas.GetHeight(), Atlas::BPP, atlas.GetBuffer(), atlas.GetWidth() * Atlas::STRIDE_BYTES);
	else if (strcmp(image_format, "bmp"))
		stbi_write_bmp(texture_filepath, atlas.GetWidth(), atlas.GetHeight(), Atlas::BPP, atlas.GetBuffer());
	else if (strcmp(image_format, "jpg"))
		stbi_write_jpg(texture_filepath, atlas.GetWidth(), atlas.GetHeight(), Atlas::BPP, atlas.GetBuffer(), jpg_quality);
	else if (strcmp(image_format, "tga"))
		stbi_write_tga(texture_filepath, atlas.GetWidth(), atlas.GetHeight(), Atlas::BPP, atlas.GetBuffer());
	else
		return false;

	return true;
}

Atlas loadAtlas(const char* asset_filepath)
{
	// TODO load atlas
	return Atlas({});
}
