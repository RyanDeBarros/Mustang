#pragma once

#include <glm/glm.hpp>
#include <string>

#include "Typedefs.h"

namespace _RendererSettings
{
	extern const char* settings_filepath;
	extern std::string rect_renderable_filepath;
	extern float initial_window_width, initial_window_height;
	extern TextureSlot max_texture_slots;
	extern VertexSize standard_vertex_pool_size;
	extern VertexSize standard_index_pool_size;
	extern glm::vec4 gl_clear_color;

	extern const char* standard_shader8_assetfile;
	extern const char* standard_shader32_assetfile;
}
