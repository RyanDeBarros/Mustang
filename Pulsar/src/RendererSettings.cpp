#include "RendererSettings.h"

namespace _RendererSettings
{
	const char* settings_filepath = "config/Renderer.toml";
	std::string quad_renderable_filepath = "config/QuadRenderable.toml";
	float window_width = 1440, window_height = 1080;
	TextureSlot max_texture_slots = 2;

	const char* standard_shader8_assetfile = "config/StandardShader8.toml";
	const char* standard_shader32_assetfile = "config/StandardShader32.toml";
}
