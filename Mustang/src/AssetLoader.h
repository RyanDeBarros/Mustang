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

LOAD_STATUS loadShader(const char* filepath, ShaderHandle& handle);
LOAD_STATUS loadTexture(const char* filepath, TextureHandle& handle);
LOAD_STATUS loadRenderable(const char* filepath, struct Renderable& renderable);
