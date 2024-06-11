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

	ShaderFactory() = delete;
	~ShaderFactory() = delete;

public:
	static void Init();
	static void Terminate();
	static ShaderHandle GetHandle(const char* vertex_shader, const char* fragment_shader);
	static void Bind(ShaderHandle handle);
	static void Unbind();

	static void SetUniform1f(ShaderHandle handle, const char* uniform_name, GLfloat v0);
	static void SetUniform1iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count = 1);
	static void SetUniformMatrix3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
};
