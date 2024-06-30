#include "AssetLoader.h"

#include <toml/toml.hpp>
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

static LOAD_STATUS verify_header(const toml::v3::ex::parse_result& file, const char* header_name)
{
	if (auto header = file["header"].value<std::string>())
	{
		return header.value() == header_name ? LOAD_STATUS::OK : LOAD_STATUS::MISMATCH_ERR;
	}
	else return LOAD_STATUS::SYNTAX_ERR;
}

bool _LoadRendererSettings()
{
	try
	{
		auto file = toml::parse_file(_RendererSettings::settings_filepath);
		if (verify_header(file, "renderer") != LOAD_STATUS::OK)
		{
			Logger::LogErrorFatal("Cannot parse renderer settings file. \"" + std::string(_RendererSettings::settings_filepath) + "\" does not have the proper header.");
			return false;
		}

		if (auto window = file["window"])
		{
			if (auto width = window["width"].value<int64_t>())
				_RendererSettings::initial_window_width = (int)width.value();
			if (auto height = window["height"].value<int64_t>())
				_RendererSettings::initial_window_height = (int)height.value();
			if (auto gl_clear_color = window["gl_clear_color"].as_array())
			{
				_RendererSettings::gl_clear_color = {
					gl_clear_color->get_as<double>(0)->get(), gl_clear_color->get_as<double>(1)->get(),
					gl_clear_color->get_as<double>(2)->get(), gl_clear_color->get_as<double>(3)->get()
				};
			}
		}
		if (auto rendering = file["rendering"])
		{
			if (auto max_texture_slots = rendering["max_texture_slots"].value<int64_t>())
				_RendererSettings::max_texture_slots = (TextureSlot)max_texture_slots.value();
			if (auto standard_vertex_pool_size = rendering["standard_vertex_pool_size"].value<int64_t>())
				_RendererSettings::standard_vertex_pool_size = (VertexSize)standard_vertex_pool_size.value();
			if (auto standard_index_pool_size = rendering["standard_index_pool_size"].value<int64_t>())
				_RendererSettings::standard_index_pool_size = (VertexSize)standard_index_pool_size.value();
			if (auto standard_shader_filepath = rendering["standard_shader"].value<std::string>())
				_RendererSettings::standard_shader_assetfile = standard_shader_filepath.value();
			if (auto rect_renderable_filepath = rendering["rect_renderable"].value<std::string>())
				_RendererSettings::rect_renderable_filepath = rect_renderable_filepath.value();
		}
		return true;
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse renderer settings file \"" + std::string(_RendererSettings::settings_filepath) + "\":" + std::string(err.description()));
		return false;
	}
}

LOAD_STATUS loadShader(const char* filepath, ShaderHandle& handle)
{
	try
	{
		auto file = toml::parse_file(filepath);
		auto verif = verify_header(file, "shader");
		if (verif != LOAD_STATUS::OK)
			return verif;

		auto shader = file["shader"];
		if (!shader)
			return LOAD_STATUS::SYNTAX_ERR;
		auto vertex_shader = shader["vertex"].value<std::string>();
		if (!vertex_shader)
			return LOAD_STATUS::SYNTAX_ERR;
		auto fragment_shader = shader["fragment"].value<std::string>();
		if (!fragment_shader)
			return LOAD_STATUS::SYNTAX_ERR;
		handle = ShaderFactory::GetHandle(vertex_shader.value().c_str(), fragment_shader.value().c_str());
		return handle > 0 ? LOAD_STATUS::OK : LOAD_STATUS::ASSET_LOAD_ERR;
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse shader asset file \"" + std::string(filepath) + "\": " + std::string(err.description()));
		return LOAD_STATUS::READ_ERR;
	}
}

LOAD_STATUS loadTexture(const char* filepath, TextureHandle& handle, const bool& new_texture, const bool& temporary_buffer)
{
	try
	{
		auto file = toml::parse_file(filepath);
		auto verif = verify_header(file, "texture");
		if (verif != LOAD_STATUS::OK)
			return verif;

		auto texture = file["texture"];
		if (!texture)
			return LOAD_STATUS::SYNTAX_ERR;
		auto path = texture["path"].value<std::string>();
		if (!path)
			return LOAD_STATUS::SYNTAX_ERR;

		TextureSettings texture_settings;
		if (auto settings = texture["settings"])
		{
			if (auto min_filter = settings["min_filter"].value<int64_t>())
			{
				if (min_filter.value() < 0 || min_filter.value() >= MinFilterLookupLength)
					return LOAD_STATUS::SYNTAX_ERR;
				texture_settings.min_filter = MinFilterLookup[min_filter.value()];
			}
			if (auto mag_filter = settings["mag_filter"].value<int64_t>())
			{
				if (mag_filter.value() < 0 || mag_filter.value() >= MagFilterLookupLength)
					return LOAD_STATUS::SYNTAX_ERR;
				texture_settings.mag_filter = MagFilterLookup[mag_filter.value()];
			}
			if (auto wrap_s = settings["wrap_s"].value<int64_t>())
			{
				if (wrap_s.value() < 0 || wrap_s.value() >= TextureWrapLookupLength)
					return LOAD_STATUS::SYNTAX_ERR;
				texture_settings.wrap_s = TextureWrapLookup[wrap_s.value()];
			}
			if (auto wrap_t = settings["wrap_t"].value<int64_t>())
			{
				if (wrap_t.value() < 0 || wrap_t.value() >= TextureWrapLookupLength)
					return LOAD_STATUS::SYNTAX_ERR;
				texture_settings.wrap_t = TextureWrapLookup[wrap_t.value()];
			}
			if (auto lod_level = settings["lod_level"].value<int64_t>())
				texture_settings.lod_level = lod_level.value();
		}
		handle = TextureFactory::GetHandle(path.value().c_str(), texture_settings, new_texture, temporary_buffer);
		return handle > 0 ? LOAD_STATUS::OK : LOAD_STATUS::ASSET_LOAD_ERR;
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse texture asset file \"" + std::string(filepath) + "\": " + std::string(err.description()));
		return LOAD_STATUS::READ_ERR;
	}
}

LOAD_STATUS loadUniformLexicon(const char* filepath, UniformLexiconHandle& handle)
{
	try
	{
		auto file = toml::parse_file(filepath);
		auto verif = verify_header(file, "uniform_lexicon");
		if (verif != LOAD_STATUS::OK)
			return verif;

		auto array = file["uniform"].as_array();
		if (!array)
			return LOAD_STATUS::SYNTAX_ERR;
		std::map<std::string, Uniform> uniform_map;
		array->for_each([&uniform_map](auto&& uniform)
		{
				if constexpr (toml::is_table<decltype(uniform)>)
				{
					auto type = uniform["type"].value<int64_t>();
					auto name = uniform["name"].value<std::string>();
					if (!type || !name)
						return LOAD_STATUS::SYNTAX_ERR;
					Uniform u;
					switch (type.value())
					{
					case 0:
					{
						auto a = uniform["value"].value<int64_t>();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						u = static_cast<GLint>(a.value());
						break;
					}
					case 1:
					{
						auto a = uniform["value"].as_array();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						u = glm::ivec2(a->get_as<int64_t>(0)->get(), a->get_as<int64_t>(1)->get());
						break;
					}
					case 2:
					{
						auto a = uniform["value"].as_array();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						u = glm::ivec3(a->get_as<int64_t>(0)->get(), a->get_as<int64_t>(1)->get(), a->get_as<int64_t>(2)->get());
						break;
					}
					case 3:
					{
						auto a = uniform["value"].as_array();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						u = glm::ivec4(a->get_as<int64_t>(0)->get(), a->get_as<int64_t>(1)->get(), a->get_as<int64_t>(2)->get(), a->get_as<int64_t>(3)->get());
						break;
					}
					case 4:
					{
						auto a = uniform["value"].value<int64_t>();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						u = static_cast<GLuint>(a.value());
						break;
					}
					case 5:
					{
						auto a = uniform["value"].as_array();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						u = glm::uvec2(a->get_as<int64_t>(0)->get(), a->get_as<int64_t>(1)->get());
						break;
					}
					case 6:
					{
						auto a = uniform["value"].as_array();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						u = glm::uvec3(a->get_as<int64_t>(0)->get(), a->get_as<int64_t>(1)->get(), a->get_as<int64_t>(2)->get());
						break;
					}
					case 7:
					{
						auto a = uniform["value"].as_array();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						u = glm::uvec4(a->get_as<int64_t>(0)->get(), a->get_as<int64_t>(1)->get(), a->get_as<int64_t>(2)->get(), a->get_as<int64_t>(3)->get());
						break;
					}
					case 8:
					{
						auto a = uniform["value"].value<double>();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						u = static_cast<GLfloat>(a.value());
						break;
					}
					case 9:
					{
						auto a = uniform["value"].as_array();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						u = glm::vec2(static_cast<float>(a->get_as<double>(0)->get()), static_cast<float>(a->get_as<double>(1)->get()));
						break;
					}
					case 10:
					{
						auto a = uniform["value"].as_array();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						u = glm::vec3(static_cast<float>(a->get_as<double>(0)->get()), static_cast<float>(a->get_as<double>(1)->get()), static_cast<float>(a->get_as<double>(2)->get()));
						break;
					}
					case 11:
					{
						auto a = uniform["value"].as_array();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						u = glm::vec4(static_cast<float>(a->get_as<double>(0)->get()), static_cast<float>(a->get_as<double>(1)->get()), static_cast<float>(a->get_as<double>(2)->get()), static_cast<float>(a->get_as<double>(3)->get()));
						break;
					}
					case 12:
					{
						auto a = uniform["value"].as_array();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						auto col0 = a->get_as<toml::array>(0)->as_array();
						auto col1 = a->get_as<toml::array>(1)->as_array();
						if (!col0 || !col1)
							return LOAD_STATUS::SYNTAX_ERR;
						u = glm::mat2(static_cast<float>(col0->get_as<double>(0)->get()), static_cast<float>(col0->get_as<double>(1)->get()), static_cast<float>(col1->get_as<double>(0)->get()), static_cast<float>(col1->get_as<double>(1)->get()));
						break;
					}
					case 13:
					{
						auto a = uniform["value"].as_array();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						auto col0 = a->get_as<toml::array>(0)->as_array();
						auto col1 = a->get_as<toml::array>(1)->as_array();
						auto col2 = a->get_as<toml::array>(2)->as_array();
						if (!col0 || !col1 || !col2)
							return LOAD_STATUS::SYNTAX_ERR;
						u = glm::mat3(
							static_cast<float>(col0->get_as<double>(0)->get()), static_cast<float>(col0->get_as<double>(1)->get()), static_cast<float>(col0->get_as<double>(2)->get()),
							static_cast<float>(col1->get_as<double>(0)->get()), static_cast<float>(col1->get_as<double>(1)->get()), static_cast<float>(col1->get_as<double>(2)->get()),
							static_cast<float>(col2->get_as<double>(0)->get()), static_cast<float>(col2->get_as<double>(1)->get()), static_cast<float>(col2->get_as<double>(2)->get())
						);
						break;
					}
					case 14:
					{
						auto a = uniform["value"].as_array();
						if (!a)
							return LOAD_STATUS::SYNTAX_ERR;
						auto col0 = a->get_as<toml::array>(0)->as_array();
						auto col1 = a->get_as<toml::array>(1)->as_array();
						auto col2 = a->get_as<toml::array>(2)->as_array();
						auto col3 = a->get_as<toml::array>(3)->as_array();
						if (!col0 || !col1 || !col2 || !col3)
							return LOAD_STATUS::SYNTAX_ERR;
						u = glm::mat4(
							static_cast<float>(col0->get_as<double>(0)->get()), static_cast<float>(col0->get_as<double>(1)->get()), static_cast<float>(col0->get_as<double>(2)->get()), static_cast<float>(col0->get_as<double>(3)->get()),
							static_cast<float>(col1->get_as<double>(0)->get()), static_cast<float>(col1->get_as<double>(1)->get()), static_cast<float>(col1->get_as<double>(2)->get()), static_cast<float>(col1->get_as<double>(3)->get()),
							static_cast<float>(col2->get_as<double>(0)->get()), static_cast<float>(col2->get_as<double>(1)->get()), static_cast<float>(col2->get_as<double>(2)->get()), static_cast<float>(col2->get_as<double>(3)->get()),
							static_cast<float>(col3->get_as<double>(0)->get()), static_cast<float>(col3->get_as<double>(1)->get()), static_cast<float>(col3->get_as<double>(2)->get()), static_cast<float>(col3->get_as<double>(3)->get())
						);
						break;
					}
					}
					uniform_map.insert({ name.value(), u });
				}
				else return LOAD_STATUS::SYNTAX_ERR;
		});
		handle = UniformLexiconFactory::GetHandle(uniform_map);
		return LOAD_STATUS::OK;
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse uniform lexicon asset file \"" + std::string(filepath) + "\": " + std::string(err.description()));
		return LOAD_STATUS::READ_ERR;
	}
}

LOAD_STATUS loadRenderable(const char* filepath, Renderable& renderable, const bool& new_texture, const bool& temporary_buffer)
{
	try
	{
		auto file = toml::parse_file(filepath);
		auto verif = verify_header(file, "renderable");
		if (verif != LOAD_STATUS::OK)
			return verif;

		auto render = file["renderable"];
		if (!render)
			return LOAD_STATUS::SYNTAX_ERR;

		TextureHandle texture_handle = 0;
		if (auto texture = render["texture"].value<std::string>())
		{
			if (loadTexture(texture.value().c_str(), texture_handle, new_texture, temporary_buffer) != LOAD_STATUS::OK)
				return LOAD_STATUS::REFERENCE_ERROR;
		}
		renderable.textureHandle = texture_handle;

		auto model = render["model"];
		if (!model)
			return LOAD_STATUS::SYNTAX_ERR;
		auto layout = model["layout"].value<int64_t>();
		auto mask = model["mask"].value<int64_t>();
		if (!layout || !mask)
			return LOAD_STATUS::SYNTAX_ERR;
		renderable.model.layout = static_cast<VertexLayout>(layout.value());
		renderable.model.layoutMask = static_cast<VertexLayoutMask>(mask.value());

		ShaderHandle shader_handle = 0;
		if (auto shader = model["shader"].value<std::string>())
		{
			if (loadShader(shader.value().c_str(), shader_handle) != LOAD_STATUS::OK)
				return LOAD_STATUS::REFERENCE_ERROR;
		}
		renderable.model.shader = shader_handle;

		UniformLexiconHandle lexicon_handle = 0;
		if (auto uniform_lexicon = model["uniform_lexicon"].value<std::string>())
		{
			if (loadUniformLexicon(uniform_lexicon.value().c_str(), lexicon_handle) != LOAD_STATUS::OK)
				return LOAD_STATUS::REFERENCE_ERROR;
		}
		renderable.model.uniformLexicon = lexicon_handle;

		auto vertex_array = render["vertices"].as_array();
		auto num_vertices = render["num_vertices"].value<int64_t>();
		if (!vertex_array || !num_vertices)
			return LOAD_STATUS::SYNTAX_ERR;
		renderable.vertexCount = static_cast<BufferCounter>(num_vertices.value());
		if (!renderable.AttachVertexBuffer(vertex_array, Render::VertexBufferLayoutCount(renderable)))
			return LOAD_STATUS::ASSET_LOAD_ERR;

		auto index_array = render["indices"].as_array();
		if (!index_array)
			return LOAD_STATUS::SYNTAX_ERR;
		renderable.indexCount = index_array->size();
		
		if (!renderable.AttachIndexBuffer(index_array, renderable.indexCount))
			return LOAD_STATUS::ASSET_LOAD_ERR;
		
		return LOAD_STATUS::OK;
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse renderable asset file \"" + std::string(filepath) + "\": " + std::string(err.description()));
		return LOAD_STATUS::READ_ERR;
	}
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
