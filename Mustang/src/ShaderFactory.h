#pragma once

class ShaderFactory
{
public:
	static unsigned int GetHandle(const char* vertex_shader, const char* fragment_shader);
	static void Bind(unsigned int handle);
	static void Unbind(unsigned int handle);
	static void Terminate();
};
