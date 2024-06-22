#include "RendererSettings.h"

namespace _RendererSettings
{
	const char* settings_filepath = "config/Renderer.toml";
	std::string rect_renderable_filepath = "config/RectRenderable.toml";
	std::string atlas_renderable_filepath = "config/AtlasRenderable.toml";
	int initial_window_width = 1440, initial_window_height = 1080;
	TextureSlot max_texture_slots = 2;
	VertexSize standard_vertex_pool_size = 2048;
	VertexSize standard_index_pool_size = 1024;
	glm::vec4 gl_clear_color = glm::vec4(0.08, 0.08, 0.08, 0.0);

	const char* standard_shader8_assetfile = "config/StandardShader8.toml";
	const char* standard_shader32_assetfile = "config/StandardShader32.toml";
	const char* standard_atlas_shader8_assetfile = "config/StandardAtlasShader8.toml";
	const char* standard_atlas_shader32_assetfile = "config/StandardAtlasShader32.toml";
}
