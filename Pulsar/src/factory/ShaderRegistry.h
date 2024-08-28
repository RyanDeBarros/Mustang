#pragma once

#include <GL/glew.h>

#include <unordered_map>

#include "Shader.h"
#include "Handles.inl"

struct ShaderConstructArgs
{
	std::string vertexShader;
	std::string fragmentShader;

	inline bool operator==(const ShaderConstructArgs& args) const
	{
		return vertexShader == args.vertexShader && fragmentShader == args.fragmentShader;
	}
};

template<>
struct std::hash<ShaderConstructArgs>
{
	inline size_t operator()(const ShaderConstructArgs& args) const
	{
		auto hash1 = hash<std::string>{}(args.vertexShader);
		auto hash2 = hash<std::string>{}(args.fragmentShader);
		return hash1 ^ (hash2 << 1);
	}
};

class ShaderRegistry
{
	static ShaderHandle handle_cap;
	static std::unordered_map<ShaderHandle, Shader> registry;
	static std::unordered_map<ShaderConstructArgs, ShaderHandle> lookup_map;

	ShaderRegistry() = delete;
	ShaderRegistry(const ShaderRegistry&) = delete;
	ShaderRegistry(ShaderRegistry&&) = delete;
	~ShaderRegistry() = delete;
	
	friend class Renderer;
	static void Init();
	static void Terminate();

	friend struct BatchModel;
	friend class RectRender;
	friend class Atlas;
	friend class TileMap;
	static ShaderHandle standard_shader;

public:
	static ShaderHandle GetHandle(const ShaderConstructArgs& args);
	static Shader const* Get(ShaderHandle handle);
	static bool DestroyShader(ShaderHandle handle);
	static void Bind(ShaderHandle handle);
	static void Unbind();
	inline static ShaderHandle Standard() { return standard_shader; }

	static void SetUniform1i(ShaderHandle handle, const char* uniform_name, const GLint value);
	static void SetUniform2iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count = 1);
	static void SetUniform3iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count = 1);
	static void SetUniform4iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count = 1);
	static void SetUniform1ui(ShaderHandle handle, const char* uniform_name, const GLuint value);
	static void SetUniform2uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count = 1);
	static void SetUniform3uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count = 1);
	static void SetUniform4uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count = 1);
	static void SetUniform1f(ShaderHandle handle, const char* uniform_name, const GLfloat value);
	static void SetUniform2fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	static void SetUniform3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	static void SetUniform4fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	static void SetUniformMatrix2fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	static void SetUniformMatrix3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
	static void SetUniformMatrix4fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count = 1);
};
