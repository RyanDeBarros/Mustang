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

bool _LoadRendererSettings();

// TODO put all this in singleton Loader class, for easier friend declarations.

LOAD_STATUS loadShader(const char* filepath, ShaderHandle& handle);
LOAD_STATUS loadTexture(const char* filepath, TextureHandle& handle, bool new_texture = false, bool temporary_buffer = false);
LOAD_STATUS loadUniformLexicon(const char* filepath, UniformLexiconHandle& handle);
LOAD_STATUS loadRenderable(const char* filepath, struct Renderable& renderable, bool new_texture = false, bool temporary_buffer = false);

bool saveAtlas(const class Atlas& atlas, const char* texture_filepath, const char* asset_filepath, const char* image_format = "png", unsigned char jpg_quality = 100);
LOAD_STATUS loadAtlas(const char* asset_filepath, TileHandle& handle);
LOAD_STATUS loadTileMap(const char* asset_filepath, std::shared_ptr<class TileMap>& tilemap);
