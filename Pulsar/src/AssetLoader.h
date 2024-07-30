#pragma once

#include <memory>

#include "Typedefs.h"

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
	static bool _LoadRendererSettings();

	static LOAD_STATUS loadShader(const char* filepath, ShaderHandle& handle);
	static LOAD_STATUS loadTexture(const char* filepath, TextureHandle& handle, bool new_texture = false, bool temporary_buffer = false);
	static LOAD_STATUS loadUniformLexicon(const char* filepath, UniformLexiconHandle& handle);
	static LOAD_STATUS loadRenderable(const char* filepath, struct Renderable& renderable, bool new_texture = false, bool temporary_buffer = false);
	
	static bool saveAtlas(const class Atlas& atlas, const char* texture_filepath, const char* asset_filepath, const char* image_format = "png", unsigned char jpg_quality = 100);
	static LOAD_STATUS loadAtlas(const char* asset_filepath, TileHandle& handle);
	static LOAD_STATUS loadTileMap(const char* asset_filepath, std::shared_ptr<class TileMap>& tilemap);
};
