#include "AssetLoader.h"

#include <toml/toml.hpp>
#include <glm/glm.hpp>
#include <stb/stb_image_write.h>

#include <unordered_map>
#include <fstream>

#include "Logger.inl"
#include "RendererSettings.h"
#include "factory/ShaderRegistry.h"
#include "factory/TextureFactory.h"
#include "factory/UniformLexiconFactory.h"
#include "factory/UniformLexicon.h"
#include "factory/Atlas.h"
#include "render/Renderable.h"
#include "render/actors/TileMap.h"
#include "render/actors/particles/ParticleSubsystemArray.h"
#include "render/actors/particles/ParticleSystem.h"
#include "render/actors/particles/ParticleSubsystemRegistry.h"
#include "utils/Data.h"

// TODO rename all macros to have PULSAR_ prefix
#define VERIFY(loadFunc) \
	auto verif = loadFunc;\
	if (verif != LOAD_STATUS::OK)\
		return verif;

// TODO instead of simply returning invalid LOAD_STATUS, also print reason for the invalidity

// TODO fickle type for all assets

static LOAD_STATUS verify_header(const toml::v3::ex::parse_result& file, const char* header_name)
{
	if (auto header = file["header"].value<std::string>())
	{
		return header.value() == header_name ? LOAD_STATUS::OK : LOAD_STATUS::MISMATCH_ERR;
	}
	else return LOAD_STATUS::SYNTAX_ERR;
}

bool Loader::_LoadRendererSettings()
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
			if (auto solid_polygon_shader = rendering["solid_polygon_shader"].value<std::string>())
				_RendererSettings::solid_polygon_shader = solid_polygon_shader.value();
			if (auto rect_renderable_filepath = rendering["rect_renderable"].value<std::string>())
				_RendererSettings::rect_renderable_filepath = rect_renderable_filepath.value();
			if (auto solid_polygon = rendering["solid_polygon"].value<std::string>())
				_RendererSettings::solid_polygon_filepath = solid_polygon.value();
			if (auto solid_point = rendering["solid_point"].value<std::string>())
				_RendererSettings::solid_point_filepath = solid_point.value();
			if (auto solid_circle = rendering["solid_circle"].value<std::string>())
				_RendererSettings::solid_circle_filepath = solid_circle.value();
			if (auto particle_frame_length = rendering["particle_frame_length"].value<double>())
				_RendererSettings::particle_frame_length = static_cast<real>(particle_frame_length.value());
		}
		return true;
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse renderer settings file \"" + std::string(_RendererSettings::settings_filepath) + "\":" + std::string(err.description()));
		return false;
	}
}

LOAD_STATUS Loader::loadShader(const char* filepath, ShaderHandle& handle)
{
	try
	{
		auto file = toml::parse_file(filepath);
		VERIFY(verify_header(file, "shader"));

		auto shader = file["shader"];
		if (!shader)
			return LOAD_STATUS::SYNTAX_ERR;
		auto vertex_shader = shader["vertex"].value<std::string>();
		if (!vertex_shader)
			return LOAD_STATUS::SYNTAX_ERR;
		auto fragment_shader = shader["fragment"].value<std::string>();
		if (!fragment_shader)
			return LOAD_STATUS::SYNTAX_ERR;
		handle = ShaderRegistry::GetHandle({ vertex_shader.value(), fragment_shader.value() });
		return handle > 0 ? LOAD_STATUS::OK : LOAD_STATUS::ASSET_LOAD_ERR;
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse shader asset file \"" + std::string(filepath) + "\": " + std::string(err.description()));
		return LOAD_STATUS::READ_ERR;
	}
}

static LOAD_STATUS readTextureSettings(const toml::v3::node_view<toml::v3::node>& settings, TextureSettings& texture_settings)
{
	if (auto min_filter = settings["min_filter"].value<int64_t>())
	{
		if (min_filter.value() < 0 || min_filter.value() >= MinFilterLookupLength)
			return LOAD_STATUS::SYNTAX_ERR;
		texture_settings.minFilter = MinFilterLookup[min_filter.value()];
	}
	if (auto mag_filter = settings["mag_filter"].value<int64_t>())
	{
		if (mag_filter.value() < 0 || mag_filter.value() >= MagFilterLookupLength)
			return LOAD_STATUS::SYNTAX_ERR;
		texture_settings.magFilter = MagFilterLookup[mag_filter.value()];
	}
	if (auto wrap_s = settings["wrap_s"].value<int64_t>())
	{
		if (wrap_s.value() < 0 || wrap_s.value() >= TextureWrapLookupLength)
			return LOAD_STATUS::SYNTAX_ERR;
		texture_settings.wrapS = TextureWrapLookup[wrap_s.value()];
	}
	if (auto wrap_t = settings["wrap_t"].value<int64_t>())
	{
		if (wrap_t.value() < 0 || wrap_t.value() >= TextureWrapLookupLength)
			return LOAD_STATUS::SYNTAX_ERR;
		texture_settings.wrapT = TextureWrapLookup[wrap_t.value()];
	}
	if (auto lod_level = settings["lod_level"].value<int64_t>())
		texture_settings.lodLevel = static_cast<GLint>(lod_level.value());
	return LOAD_STATUS::OK;
}

LOAD_STATUS Loader::loadTexture(const char* filepath, TextureHandle& handle, TextureVersion texture_version, bool temporary_buffer)
{
	try
	{
		auto file = toml::parse_file(filepath);
		VERIFY(verify_header(file, "texture"));

		auto texture = file["texture"];
		if (!texture)
			return LOAD_STATUS::SYNTAX_ERR;
		auto path = texture["path"].value<std::string>();
		if (!path)
			return LOAD_STATUS::SYNTAX_ERR;

		TextureSettings texture_settings;
		if (auto settings = texture["settings"])
		{
			VERIFY(readTextureSettings(settings, texture_settings));
		}
		handle = TextureFactory::GetHandle(TextureConstructArgs_filepath{ path.value(), texture_settings, temporary_buffer, texture_version});
		return handle > 0 ? LOAD_STATUS::OK : LOAD_STATUS::ASSET_LOAD_ERR;
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse texture asset file \"" + std::string(filepath) + "\": " + std::string(err.description()));
		return LOAD_STATUS::READ_ERR;
	}
}

LOAD_STATUS Loader::loadUniformLexicon(const char* filepath, UniformLexiconHandle& handle)
{
	try
	{
		auto file = toml::parse_file(filepath);
		VERIFY(verify_header(file, "uniform_lexicon"));

		auto array = file["uniform"].as_array();
		if (!array)
			return LOAD_STATUS::SYNTAX_ERR;
		std::unordered_map<std::string, Uniform> uniform_map;
		LOAD_STATUS status = LOAD_STATUS::OK;
		array->for_each([&uniform_map, &status](auto&& uniform)
		{
				if constexpr (toml::is_table<decltype(uniform)>)
				{
					auto type = uniform["type"].value<int64_t>();
					auto name = uniform["name"].value<std::string>();
					if (!type || !name)
					{
						status = LOAD_STATUS::SYNTAX_ERR;
						return;
					}
					Uniform u;
					switch (type.value())
					{
					case 0:
					{
						auto a = uniform["value"].value<int64_t>();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						u = static_cast<GLint>(a.value());
						break;
					}
					case 1:
					{
						auto a = uniform["value"].as_array();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						u = glm::ivec2(a->get_as<int64_t>(0)->get(), a->get_as<int64_t>(1)->get());
						break;
					}
					case 2:
					{
						auto a = uniform["value"].as_array();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						u = glm::ivec3(a->get_as<int64_t>(0)->get(), a->get_as<int64_t>(1)->get(), a->get_as<int64_t>(2)->get());
						break;
					}
					case 3:
					{
						auto a = uniform["value"].as_array();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						u = glm::ivec4(a->get_as<int64_t>(0)->get(), a->get_as<int64_t>(1)->get(), a->get_as<int64_t>(2)->get(), a->get_as<int64_t>(3)->get());
						break;
					}
					case 4:
					{
						auto a = uniform["value"].value<int64_t>();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						u = static_cast<GLuint>(a.value());
						break;
					}
					case 5:
					{
						auto a = uniform["value"].as_array();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						u = glm::uvec2(a->get_as<int64_t>(0)->get(), a->get_as<int64_t>(1)->get());
						break;
					}
					case 6:
					{
						auto a = uniform["value"].as_array();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						u = glm::uvec3(a->get_as<int64_t>(0)->get(), a->get_as<int64_t>(1)->get(), a->get_as<int64_t>(2)->get());
						break;
					}
					case 7:
					{
						auto a = uniform["value"].as_array();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						u = glm::uvec4(a->get_as<int64_t>(0)->get(), a->get_as<int64_t>(1)->get(), a->get_as<int64_t>(2)->get(), a->get_as<int64_t>(3)->get());
						break;
					}
					case 8:
					{
						auto a = uniform["value"].value<double>();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						u = static_cast<GLfloat>(a.value());
						break;
					}
					case 9:
					{
						auto a = uniform["value"].as_array();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						u = glm::vec2(static_cast<float>(a->get_as<double>(0)->get()), static_cast<float>(a->get_as<double>(1)->get()));
						break;
					}
					case 10:
					{
						auto a = uniform["value"].as_array();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						u = glm::vec3(static_cast<float>(a->get_as<double>(0)->get()), static_cast<float>(a->get_as<double>(1)->get()), static_cast<float>(a->get_as<double>(2)->get()));
						break;
					}
					case 11:
					{
						auto a = uniform["value"].as_array();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						u = glm::vec4(static_cast<float>(a->get_as<double>(0)->get()), static_cast<float>(a->get_as<double>(1)->get()), static_cast<float>(a->get_as<double>(2)->get()), static_cast<float>(a->get_as<double>(3)->get()));
						break;
					}
					case 12:
					{
						auto a = uniform["value"].as_array();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						auto col0 = a->get_as<toml::array>(0)->as_array();
						auto col1 = a->get_as<toml::array>(1)->as_array();
						if (!col0 || !col1)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						u = glm::mat2(static_cast<float>(col0->get_as<double>(0)->get()), static_cast<float>(col0->get_as<double>(1)->get()), static_cast<float>(col1->get_as<double>(0)->get()), static_cast<float>(col1->get_as<double>(1)->get()));
						break;
					}
					case 13:
					{
						auto a = uniform["value"].as_array();
						if (!a)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						auto col0 = a->get_as<toml::array>(0)->as_array();
						auto col1 = a->get_as<toml::array>(1)->as_array();
						auto col2 = a->get_as<toml::array>(2)->as_array();
						if (!col0 || !col1 || !col2)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
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
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
						auto col0 = a->get_as<toml::array>(0)->as_array();
						auto col1 = a->get_as<toml::array>(1)->as_array();
						auto col2 = a->get_as<toml::array>(2)->as_array();
						auto col3 = a->get_as<toml::array>(3)->as_array();
						if (!col0 || !col1 || !col2 || !col3)
						{
							status = LOAD_STATUS::SYNTAX_ERR;
							return;
						}
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
				else
				{
					status = LOAD_STATUS::SYNTAX_ERR;
					return;
				}
		});
		if (status != LOAD_STATUS::OK)
			return status;
		handle = UniformLexiconFactory::GetHandle({ uniform_map });
		return LOAD_STATUS::OK;
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse uniform lexicon asset file \"" + std::string(filepath) + "\": " + std::string(err.description()));
		return LOAD_STATUS::READ_ERR;
	}
}

LOAD_STATUS Loader::loadRenderable(const char* filepath, Renderable& renderable, TextureVersion texture_version, bool temporary_buffer)
{
	try
	{
		auto file = toml::parse_file(filepath);
		VERIFY(verify_header(file, "renderable"));

		auto render = file["renderable"];
		if (!render)
			return LOAD_STATUS::SYNTAX_ERR;

		TextureHandle texture_handle = 0;
		if (auto texture = render["texture"].value<std::string>())
		{
			if (loadTexture(texture.value().c_str(), texture_handle, texture_version, temporary_buffer) != LOAD_STATUS::OK)
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
		renderable.indexCount = static_cast<BufferCounter>(index_array->size());
		
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

bool Loader::saveAtlas(const Atlas& atlas, const char* texture_filepath, const char* asset_filepath, const char* image_format, unsigned char jpg_quality)
{
	try
	{
		std::ofstream f(asset_filepath);
		if (!f.is_open())
		{
			Logger::LogErrorFatal("Cannot open atlas asset file \"" + std::string(asset_filepath) + "\".");
			return false;
		}

		toml::table tbl;
		tbl.insert_or_assign("header", "atlas");

		toml::table atlas_;
		atlas_.insert_or_assign("path", texture_filepath);
		atlas_.insert_or_assign("border", atlas.GetBorder());

		toml::array placements;
		for (size_t i = 0; i < atlas.GetPlacements().size(); i++)
		{
			const Placement& p = atlas.GetPlacements()[i];
			toml::table placement_table;
			placement_table.insert_or_assign("x", static_cast<int64_t>(p.x));
			placement_table.insert_or_assign("y", static_cast<int64_t>(p.y));
			placement_table.insert_or_assign("w", static_cast<int64_t>(p.w));
			placement_table.insert_or_assign("h", static_cast<int64_t>(p.h));
			placement_table.insert_or_assign("r", static_cast<int64_t>(p.r));
			placements.push_back(placement_table);
		}
		atlas_.insert_or_assign("placement", placements);

		tbl.insert_or_assign("atlas", atlas_);
		f << tbl;
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse atlas asset file \"" + std::string(asset_filepath) + "\": " + std::string(err.description()));
		return false;
	}
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

LOAD_STATUS Loader::loadAtlas(const char* asset_filepath, Atlas*& atlas_initializer)
{
	try
	{
		auto file = toml::parse_file(asset_filepath);
		auto verif = verify_header(file, "atlas");
		if (verif != LOAD_STATUS::OK)
			return verif;

		auto atlas = file["atlas"];
		if (!atlas)
			return LOAD_STATUS::SYNTAX_ERR;
		auto path = atlas["path"].value<std::string>();
		if (!path)
			return LOAD_STATUS::SYNTAX_ERR;
		auto _border = atlas["border"].value<int64_t>();
		int border = _border ? static_cast<int>(_border.value()) : 0;

		auto p_arr = atlas["placement"].as_array();
		std::vector<Placement> placements;
		size_t i = 0;
		while (auto table = p_arr->get_as<toml::table>(i))
		{
			Placement placement{};
			auto x = (*table)["x"].value<int64_t>();
			auto y = (*table)["y"].value<int64_t>();
			auto w = (*table)["w"].value<int64_t>();
			auto h = (*table)["h"].value<int64_t>();
			auto r = (*table)["r"].value<bool>();
			if (!x || !y || !w || !h || !r)
				return LOAD_STATUS::SYNTAX_ERR;
			placement.x = static_cast<int>(x.value());
			placement.y = static_cast<int>(y.value());
			placement.w = static_cast<int>(w.value());
			placement.h = static_cast<int>(h.value());
			placement.r = r.value();
			placements.push_back(placement);
			i++;
		}

		try
		{
			atlas_initializer = new Atlas(path.value(), std::move(placements), border);
			return LOAD_STATUS::OK;
		}
		catch (const null_handle_error&)
		{
			// no need to delete atlas_initializer. If construction fails, atlas_initializer will still be null.
			return LOAD_STATUS::ASSET_LOAD_ERR;
		}
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse atlas asset file \"" + std::string(asset_filepath) + "\": " + std::string(err.description()));
		return LOAD_STATUS::READ_ERR;
	}
}

LOAD_STATUS Loader::loadTileMap(const char* asset_filepath, TileMap*& tilemap_initializer, TextureVersion texture_version)
{
	try
	{
		auto file = toml::parse_file(asset_filepath);
		VERIFY(verify_header(file, "tilemap"));

		auto tm = file["tilemap"];
		if (!tm)
			return LOAD_STATUS::SYNTAX_ERR;

		Atlas* atlas = nullptr;
		auto atlas_filepath = tm["atlas"].value<std::string>();
		if (!atlas_filepath)
			return LOAD_STATUS::SYNTAX_ERR;
		if (loadAtlas(atlas_filepath.value().c_str(), atlas) != LOAD_STATUS::OK)
			return LOAD_STATUS::REFERENCE_ERROR;

		TextureSettings texture_settings = Texture::nearest_settings;
		if (auto settings = tm["texture_settings"])
		{
			VERIFY(readTextureSettings(settings, texture_settings));
		}

		ShaderHandle shader = ShaderRegistry::Standard();
		if (auto sh = tm["shader"].value<std::string>())
		{
			if (loadShader(sh.value().c_str(), shader) != LOAD_STATUS::OK)
				return LOAD_STATUS::REFERENCE_ERROR;
		}

		ZIndex z = 0;
		if (auto zin = tm["z"].value<int64_t>())
			z = static_cast<ZIndex>(zin.value());

		bool visible = true;
		if (auto vis = tm["visible"].value<bool>())
			visible = vis.value();

		FickleType fickle_type = FickleType::Transformable;
		if (auto ft = tm["fickle"].value<int64_t>())
		{
			switch (ft.value())
			{
			case 0:
				fickle_type = FickleType::Protean;
				break;
			case 1:
				fickle_type = FickleType::Transformable;
				break;
			case 2:
				fickle_type = FickleType::Modulatable;
				break;
			}
		}

		tilemap_initializer = new TileMap(std::shared_ptr<Atlas>(atlas), texture_settings, texture_version, shader, z, fickle_type, visible);

		std::vector<size_t> ordering;
		if (auto order = tm["ordering"].as_array())
		{
			for (auto i = 0; i < order->size(); i++)
				ordering.push_back(static_cast<size_t>(order->get_as<int64_t>(i)->get()));
		}
		if (!ordering.empty())
		{
			try
			{
				if (!tilemap_initializer->SetOrdering(ordering))
					return LOAD_STATUS::ASSET_LOAD_ERR;
			}
			catch (bad_permutation_error)
			{
				return LOAD_STATUS::ASSET_LOAD_ERR;
			}
		}

		if (auto tiles = tm["tiles"].as_array())
		{
			for (auto i = 0; i < tiles->size(); i++)
			{
				if (auto tile = tiles->get_as<toml::array>(i))
				{
					tilemap_initializer->Insert(
						static_cast<size_t>(tile->get_as<int64_t>(0)->get()),
						static_cast<float>(tile->get_as<double>(1) ? tile->get_as<double>(1)->get() : tile->get_as<int64_t>(1)->get()),
						static_cast<float>(tile->get_as<double>(2) ? tile->get_as<double>(2)->get() : tile->get_as<int64_t>(2)->get())
					);
				}
			}
		}

		return LOAD_STATUS::OK;
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse tilemap asset file \"" + std::string(asset_filepath) + "\": " + std::string(err.description()));
		return LOAD_STATUS::READ_ERR;
	}
}

LOAD_STATUS Loader::loadParticleEffect(const char* filepath, ParticleEffect*& peffect_initializer, std::string ptype, bool auto_enable)
{
	try
	{
		auto file = toml::parse_file(filepath);
		auto verif = verify_header(file, "peffect");
		if (verif != LOAD_STATUS::OK)
			return verif;

		auto peffect = file["peffect"];
		if (!peffect)
			return LOAD_STATUS::SYNTAX_ERR;
		if (ptype == "")
		{
			auto ptype_default = peffect["ptype"].value<std::string>();
			if (ptype_default)
				ptype = ptype_default.value();
			if (ptype == "")
				ptype = "array";
		}
		// TODO extract this into separate function, like what was done with texture settings.
		FickleType fickle_type = FickleType::Protean;
		if (auto fickle = peffect["fickle"].value<std::string>())
		{
			const std::string& ft = fickle.value();
			if (ft == "protean")
				fickle_type = FickleType::Protean;
			else if (ft == "transformable")
				fickle_type = FickleType::Transformable;
			else if (ft == "modulatable")
				fickle_type = FickleType::Modulatable;
		}

		auto subsys_arr = peffect["subsys"].as_array();
		std::vector<ParticleSubsystemData> subsystems;
		std::vector<std::pair<unsigned int, Transform2D>> transforms;
		std::vector<std::pair<unsigned int, Modulate>> modulates;
		size_t i = 0;
		while (auto table = subsys_arr->get_as<toml::table>(i))
		{
			auto fid = (*table)["fid"].value<std::string>();
			auto params = (*table)["args"].as_array();
			if (!fid || !params)
				return LOAD_STATUS::SYNTAX_ERR;
			Array<float> params_arr(params->size());
			for (size_t i = 0; i < params_arr.Size(); i++)
				params_arr[i] = static_cast<float>(params->get_as<double>(i)->get());
			subsystems.push_back(ParticleSubsystemRegistry::Instance().Invoke(fid.value(), params_arr));

			if (auto transform = (*table)["transform"].as_array())
			{
				auto position = transform->get_as<toml::v3::array>(0);
				if (!position)
					return LOAD_STATUS::SYNTAX_ERR;
				auto posX = position->get_as<double>(0);
				auto posY = position->get_as<double>(1);
				if (!posX || !posY)
					return LOAD_STATUS::SYNTAX_ERR;
				auto rotation = transform->get_as<double>(1);
				if (!rotation)
					return LOAD_STATUS::SYNTAX_ERR;
				auto scale = transform->get_as<toml::v3::array>(2);
				if (!scale)
					return LOAD_STATUS::SYNTAX_ERR;
				auto scX = scale->get_as<double>(0);
				auto scY = scale->get_as<double>(1);
				if (!scX || !scY)
					return LOAD_STATUS::SYNTAX_ERR;
				
				transforms.push_back({ static_cast<unsigned int>(i), {{ static_cast<float>(posX->get()), static_cast<float>(posY->get()) }, static_cast<float>(rotation->get()), { static_cast<float>(scX->get()), static_cast<float>(scY->get()) }}});
			}
			if (auto modulate = (*table)["modulate"].as_array())
			{
				auto mod = modulate->get_as<toml::v3::array>(0);
				if (!mod)
					return LOAD_STATUS::SYNTAX_ERR;
				auto modR = mod->get_as<double>(0);
				auto modG = mod->get_as<double>(1);
				auto modB = mod->get_as<double>(2);
				auto modA = mod->get_as<double>(3);
				if (!modR || !modG || !modB || !modA)
					return LOAD_STATUS::SYNTAX_ERR;

				modulates.push_back({ static_cast<unsigned int>(i), { static_cast<float>(modR->get()), static_cast<float>(modG->get()), static_cast<float>(modB->get()), static_cast<float>(modA->get()) } });
			}

			i++;
		}

		if (ptype == "array")
			peffect_initializer = new ParticleSubsystemArray(subsystems, 0, fickle_type, true, auto_enable);
		else if (ptype == "system")
			peffect_initializer = new ParticleSystem(subsystems, 0, fickle_type, true, auto_enable);
		else
			return LOAD_STATUS::ASSET_LOAD_ERR;

		for (const auto& tr : transforms)
		{
			set_ptr(peffect_initializer->SubsystemRef(tr.first).Fickler().Transform(), tr.second);
			peffect_initializer->SubsystemRef(tr.first).Fickler().SyncT();
		}
		for (const auto& mo : modulates)
		{
			set_ptr(peffect_initializer->SubsystemRef(mo.first).Fickler().Modulate(), mo.second);
			peffect_initializer->SubsystemRef(mo.first).Fickler().SyncM();
		}

		return LOAD_STATUS::OK;
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse particle effect asset file \"" + std::string(filepath) + "\": " + std::string(err.description()));
		return LOAD_STATUS::READ_ERR;
	}
}
