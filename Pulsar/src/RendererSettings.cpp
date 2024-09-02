#include "RendererSettings.h"

namespace _RendererSettings
{
	const char* settings_filepath = "config/Renderer.toml";

	int initial_window_width = 1440, initial_window_height = 1080;
	glm::vec4 gl_clear_color = glm::vec4(0.08, 0.08, 0.08, 0.0);
	bool vsync_on = true;
	
	TextureSlot max_texture_slots = 32;
	VertexSize standard_vertex_pool_size = 2048;
	VertexSize standard_index_pool_size = 1024;

	std::string standard_shader_assetfile = "config/StandardShader32.toml";
	std::string solid_polygon_shader = "config/SolidPolygonShader.toml";
	std::string rect_renderable_filepath = "config/RectRenderable.toml";
	std::string solid_polygon_filepath = "config/SolidPolygon.toml";
	std::string solid_point_filepath = "config/SolidPoint.toml";
	std::string solid_circle_filepath = "config/SolidCircle.toml";

	real particle_frame_length = 0.0167f;
}
