#pragma once

#include <glm/glm.hpp>
#include <string>

#include "Pulsar.h"

typedef unsigned int VertexSize;

class PulsarSettings
{
	PulsarSettings();
	PulsarSettings(const PulsarSettings&) = delete;
	PulsarSettings(PulsarSettings&&) = delete;
	~PulsarSettings() = default;

	static PulsarSettings& ps()
	{
		static PulsarSettings _ps;
		return _ps;
	}

public:
	static const char* settings_filepath() { return ps()._settings_filepath; }

	static bool _loaded() { return ps().__loaded; }

	static int initial_window_width() { return ps()._initial_window_width; }
	static int initial_window_height() { return ps()._initial_window_height; }
	static glm::vec4 gl_clear_color() { return ps()._gl_clear_color; }
	static bool vsync_on() { return ps()._vsync_on; }

	static TextureSlot max_texture_slots() { return ps()._max_texture_slots; }
	static VertexSize standard_vertex_pool_size() { return ps()._standard_vertex_pool_size; }
	static VertexSize standard_index_pool_size() { return ps()._standard_index_pool_size; }

	static const char* standard_shader_assetfile() { return ps()._standard_shader_assetfile.c_str(); }
	static const char* solid_polygon_shader() { return ps()._solid_polygon_shader.c_str(); }
	static const char* rect_renderable_filepath() { return ps()._rect_renderable_filepath.c_str(); }
	static const char* solid_polygon_filepath() { return ps()._solid_polygon_filepath.c_str(); }
	static const char* solid_point_filepath() { return ps()._solid_point_filepath.c_str(); }
	static const char* solid_circle_filepath() { return ps()._solid_circle_filepath.c_str(); }

	static real particle_frame_length() { return ps()._particle_frame_length;}

	static const char* sdl_gamecontrollerdb() { return ps()._sdl_gamecontrollerdb.c_str(); }

	static glm::vec2 initial_window_rel_pos(const float& x, const float& y) { return ps()._initial_window_rel_pos(x, y); }

private:
	const char* _settings_filepath = "config/Renderer.toml";

	bool __loaded = true;

	int _initial_window_width = 1440;
	int _initial_window_height = 1080;
	glm::vec4 _gl_clear_color = { 0.08, 0.08, 0.08, 0.0 };
	bool _vsync_on = true;

	TextureSlot _max_texture_slots = 32;
	VertexSize _standard_vertex_pool_size = 2048;
	VertexSize _standard_index_pool_size = 1024;

	std::string _standard_shader_assetfile = "config/StandardShader32.toml";
	std::string _solid_polygon_shader = "config/SolidPolygonShader.toml";
	std::string _rect_renderable_filepath = "config/RectRenderable.toml";
	std::string _solid_polygon_filepath = "config/SolidPolygon.toml";
	std::string _solid_point_filepath = "config/SolidPoint.toml";
	std::string _solid_circle_filepath = "config/SolidCircle.toml";

	real _particle_frame_length = 0.0167f;

	std::string _sdl_gamecontrollerdb = "config/sdl-gamecontrollerdb/gamecontrollerdb.txt";

	glm::vec2 _initial_window_rel_pos(const float& x, const float& y) { return { x * _initial_window_width, y * _initial_window_height }; }
};
