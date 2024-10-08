#pragma once

#include <toml/toml.hpp>

#include <memory>
#include <string>

#include "Handles.inl"

enum class LOAD_STATUS
{
	OK,
	READ_ERR,
	MISMATCH_ERR,
	SYNTAX_ERR,
	ASSET_LOAD_ERR,
	REFERENCE_ERROR
};

struct Loader
{
	static bool _LoadRendererSettingsContent(toml::v3::parse_result& parse_result, const std::string& filepath);
	
	static LOAD_STATUS verify_header(const toml::v3::ex::parse_result& file, const char* header_name);

	static LOAD_STATUS loadShader(const char* filepath, ShaderHandle& handle);
	static LOAD_STATUS loadTexture(const char* filepath, TextureHandle& handle, TextureVersion texture_version = 0, bool temporary_buffer = true, float svg_scale = 1.0f);
	static LOAD_STATUS loadUniformLexicon(const char* filepath, UniformLexiconHandle& handle);
	static LOAD_STATUS loadRenderable(const char* filepath, struct Renderable& renderable, TextureVersion texture_version = 0, bool temporary_buffer = true);
	
	static bool saveAtlas(const class Atlas& atlas, const char* texture_filepath, const char* asset_filepath, bool flip_vertically = true, const char* image_format = "png", unsigned char jpg_quality = 100);
	/// pass nullptr to atlas parameter, and it will point to memory on the heap.
	static LOAD_STATUS loadAtlas(const char* asset_filepath, class Atlas*& atlas_initializer);
	/// pass nullptr to tilemap parameter, and it will point to memory on the heap.
	static LOAD_STATUS loadTileMap(const char* asset_filepath, class TileMap*& tilemap_initializer, TextureVersion texture_version = 0);

	static LOAD_STATUS loadParticleEffect(const char* filepath, class ParticleEffect*& peffect_initializer, std::string ptype = "", bool auto_enable = false);
};
