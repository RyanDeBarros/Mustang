#include "PulsarSettings.h"

#include "AssetLoader.h"

PulsarSettings::PulsarSettings()
{
	toml::v3::parse_result file;
	if (!Loader::_LoadRendererSettingsContent(file, _settings_filepath))
	{
		__loaded = false;
		return;
	}

	if (auto window = file["window"])
	{
		if (auto width = window["width"].value<int64_t>())
			_initial_window_width = static_cast<int>(width.value());
		if (auto height = window["height"].value<int64_t>())
			_initial_window_height = static_cast<int>(height.value());
		if (auto glcc = window["gl_clear_color"].as_array())
		{
		_gl_clear_color = {
				glcc->get_as<double>(0)->get(), glcc->get_as<double>(1)->get(),
				glcc->get_as<double>(2)->get(), glcc->get_as<double>(3)->get()
			};
		}
		if (auto vo = window["vsync_on"].value<bool>())
			_vsync_on = vo.value();
		if (auto sdlgcdb = window["sdl_gamecontrollerdb"].value<std::string>())
			_sdl_gamecontrollerdb = sdlgcdb.value();
	}

	if (auto rendering = file["rendering"])
	{
		if (auto mts = rendering["max_texture_slots"].value<int64_t>())
			_max_texture_slots = static_cast<TextureSlot>(mts.value());
		if (auto svps = rendering["standard_vertex_pool_size"].value<int64_t>())
			_standard_vertex_pool_size = static_cast<VertexSize>(svps.value());
		if (auto sips = rendering["standard_index_pool_size"].value<int64_t>())
			_standard_index_pool_size = static_cast<VertexSize>(sips.value());
		if (auto ssf = rendering["standard_shader"].value<std::string>())
			_standard_shader_assetfile = ssf.value();
		if (auto sps = rendering["solid_polygon_shader"].value<std::string>())
			_solid_polygon_shader = sps.value();
		if (auto rrf = rendering["rect_renderable"].value<std::string>())
			_rect_renderable_filepath = rrf.value();
		if (auto tsf = rendering["text_standard"].value<std::string>())
			_text_standard_filepath = tsf.value();
		if (auto sp = rendering["solid_polygon"].value<std::string>())
			_solid_polygon_filepath = sp.value();
		if (auto sp = rendering["solid_point"].value<std::string>())
			_solid_point_filepath = sp.value();
		if (auto sc = rendering["solid_circle"].value<std::string>())
			_solid_circle_filepath = sc.value();
		if (auto pfl = rendering["particle_frame_length"].value<double>())
			_particle_frame_length = static_cast<real>(pfl.value());
	}
}
