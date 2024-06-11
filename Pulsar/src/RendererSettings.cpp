#include "RendererSettings.h"

namespace _RendererSettings
{
	const char* settings_filepath = "config/Renderer.toml";
	std::string rect_renderable_filepath = "config/RectRenderable.toml";
	float window_width = 1440, window_height = 1080;
	TextureSlot max_texture_slots = 2;
	VertexSize standard_vertex_pool_size = 2048;
	VertexSize standard_index_pool_size = 1024;

	const char* standard_shader8_assetfile = "config/StandardShader8.toml";
	const char* standard_shader32_assetfile = "config/StandardShader32.toml";
}
