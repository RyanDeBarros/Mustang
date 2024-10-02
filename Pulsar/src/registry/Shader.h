#pragma once

#include <GL/glew.h>

#include <unordered_map>
#include <string>

#include "Registry.inl"
#include "Handles.inl"

typedef GLuint Shader_RID;

struct ShaderConstructArgs
{
	std::string vertexFilepath;
	std::string fragmentFilepath;

	ShaderConstructArgs(const std::string& vertex_filepath, const std::string& fragment_filepath)
		: vertexFilepath(vertex_filepath), fragmentFilepath(fragment_filepath) {}
	ShaderConstructArgs(std::string&& vertex_filepath, std::string&& fragment_filepath)
		: vertexFilepath(std::move(vertex_filepath)), fragmentFilepath(std::move(fragment_filepath)) {}

	bool operator==(const ShaderConstructArgs&) const = default;
};

template<>
struct std::hash<ShaderConstructArgs>
{
	size_t operator()(const ShaderConstructArgs& args) const
	{
		auto hash1 = hash<std::string>{}(args.vertexFilepath);
		auto hash2 = hash<std::string>{}(args.fragmentFilepath);
		return hash1 ^ (hash2 << 1);
	}
};

class Shader
{
	Shader_RID m_RID;
	mutable std::unordered_map<const char*, GLint> m_UniformLocationCache;

public:
	Shader(const ShaderConstructArgs& args);
	Shader(const Shader& shader) = delete;
	Shader(Shader&& shader) noexcept;
	Shader& operator=(Shader&& shader) noexcept;
	~Shader();

	void Bind() const;
	void Unbind() const;

	GLint GetUniformLocation(const char* uniform_name) const;
	
	operator bool() const { return m_RID > 0; }
};

class ShaderRegistry : public Registry<Shader, ShaderHandle, ShaderConstructArgs>
{
	ShaderHandle standard_shader = 0;

public:
	void DefineStandardShader();

	void Bind(ShaderHandle handle);
	void Unbind();
	ShaderHandle Standard() const { return standard_shader; }

	void SetUniform1i(ShaderHandle handle, const char* uniform_name, const GLint value);
	void SetUniform2iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count = 1);
	void SetUniform3iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count = 1);
	void SetUniform4iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count = 1);
	void SetUniform1ui(ShaderHandle handle, const char* uniform_name, const GLuint value);
	void SetUniform2uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count = 1);
	void SetUniform3uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count = 1);
	void SetUniform4uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count = 1);
	void SetUniform1f(ShaderHandle handle, const char* uniform_name, const GLfloat value);
	void SetUniform2fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	void SetUniform3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	void SetUniform4fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	void SetUniformMatrix2fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	void SetUniformMatrix3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	void SetUniformMatrix4fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
};
