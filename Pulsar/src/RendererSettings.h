#pragma once

#include <glm/glm.hpp>
#include <string>

#include "Typedefs.h"

namespace _RendererSettings
{
	extern const char* settings_filepath;
	
	extern int initial_window_width, initial_window_height;
	extern glm::vec4 gl_clear_color;
	
	extern TextureSlot max_texture_slots;
	extern VertexSize standard_vertex_pool_size;
	extern VertexSize standard_index_pool_size;

	extern std::string standard_shader_assetfile;
	extern std::string solid_polygon_shader;
	extern std::string rect_renderable_filepath;
	extern std::string solid_polygon_filepath;
	extern std::string solid_circle_filepath;

	inline glm::vec2 initial_window_rel_pos(const float& x, const float& y) { return {x * initial_window_width, y * initial_window_height}; }
}
