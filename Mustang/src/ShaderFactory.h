#pragma once

#include <unordered_map>
#include <GL/glew.h>

#include "Typedefs.h"
#include "Shader.h"

class ShaderFactory
{
	static ShaderHandle handle_cap;
	static std::unordered_map<ShaderHandle, struct ShaderElement>* factory;
	static Shader* Get(ShaderHandle);
public:
	static void Init();
	static void Terminate();
	static ShaderHandle GetHandle(const char* vertex_shader, const char* fragment_shader);
	static void Bind(ShaderHandle handle);
	static void Unbind();

	static void SetUniform1f(ShaderHandle handle, const char* uniform_name, GLfloat v0);
	static void SetUniform1iv(ShaderHandle handle, const char* uniform_name, GLsizei count, const GLint* value);
};
