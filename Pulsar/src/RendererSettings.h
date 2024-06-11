#pragma once

#include <string>

#include "Typedefs.h"

namespace _RendererSettings
{
	extern const char* settings_filepath;
	extern std::string rect_renderable_filepath;
	extern float window_width, window_height;
	extern TextureSlot max_texture_slots;
	extern VertexSize standard_vertex_pool_size;
	extern VertexSize standard_index_pool_size;

	extern const char* standard_shader8_assetfile;
	extern const char* standard_shader32_assetfile;
}
