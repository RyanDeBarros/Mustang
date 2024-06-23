#pragma once

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

LOAD_STATUS loadShader(const char* filepath, ShaderHandle& handle);
LOAD_STATUS loadTexture(const char* filepath, TextureHandle& handle, const bool& new_texture = false, const bool& temporary_buffer = false);
LOAD_STATUS loadUniformLexicon(const char* filepath, UniformLexiconHandle& handle);
LOAD_STATUS loadRenderable(const char* filepath, struct Renderable& renderable, const bool& new_texture = false, const bool& temporary_buffer = false);

// TODO
void saveAtlas(const class Atlas& atlas, const char* texture_filepath, const char* asset_filepath);
class Atlas loadAtlas(const char* asset_filepath);

