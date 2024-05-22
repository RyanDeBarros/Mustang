#pragma once

#include "ShaderFactory.h"

enum class LOAD_STATUS
{
	OK,
	READ_ERR,
	MISMATCH_ERR,
	SYNTAX_ERR
};

LOAD_STATUS loadShader(const char* filepath, ShaderHandle& handle);
