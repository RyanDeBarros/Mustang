#pragma once

#include <unordered_map>

#include "Typedefs.h"

class ShaderFactory
{
	static ShaderHandle handle_cap;
	static std::unordered_map<ShaderHandle, struct ShaderElement>* factory;
public:
	static void Init();
	static ShaderHandle GetHandle(const char* vertex_shader, const char* fragment_shader);
	static void Bind(ShaderHandle handle);
	static void Unbind();
	static void Terminate();
};
