#pragma once

#include "Typedefs.h"

enum class LOAD_STATUS
{
	OK,
	READ_ERR,
	MISMATCH_ERR,
	SYNTAX_ERR,
	ASSET_LOAD_ERR
};

// TODO use toml format for mass files
LOAD_STATUS loadShader(const char* filepath, ShaderHandle& handle);
LOAD_STATUS loadTexture(const char* filepath, TextureHandle& handle);
