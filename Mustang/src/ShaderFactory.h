#pragma once

typedef unsigned int ShaderHandle;

class ShaderFactory
{
public:
	static ShaderHandle GetHandle(const char* vertex_shader, const char* fragment_shader);
	static void Bind(ShaderHandle handle);
	static void Unbind(ShaderHandle handle);
	static void Terminate();
};
