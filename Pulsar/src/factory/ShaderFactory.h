#pragma once

#include <unordered_map>
#include <GL/glew.h>

#include "Typedefs.h"
#include "Shader.h"

class ShaderFactory
{
	static ShaderHandle handle_cap;
	static std::unordered_map<ShaderHandle, Shader*> factory;
	static Shader* Get(ShaderHandle);

	ShaderFactory() = delete;
	~ShaderFactory() = delete;

	friend class Renderer;
	static void Init();
	static void Terminate();

public:
	static ShaderHandle GetHandle(const char* vertex_shader, const char* fragment_shader);
	static void Bind(ShaderHandle handle);
	static void Unbind();

	static void SetUniform1iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count = 1);
	static void SetUniform2iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count = 1);
	static void SetUniform3iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count = 1);
	static void SetUniform4iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count = 1);
	static void SetUniform1uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count = 1);
	static void SetUniform2uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count = 1);
	static void SetUniform3uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count = 1);
	static void SetUniform4uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count = 1);
	static void SetUniform1fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	static void SetUniform2fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	static void SetUniform3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	static void SetUniform4fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	static void SetUniformMatrix2fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	static void SetUniformMatrix3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	static void SetUniformMatrix4fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
};
