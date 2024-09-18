#include "ShaderRegistry.h"

#include "Macros.h"
#include "Logger.inl"
#include "AssetLoader.h"
#include "PulsarSettings.h"

#ifndef PULSAR_ELSE_CHECK_BAD_UNIFORM
#if PULSAR_IGNORE_WARNINGS_NULL_SHADER
#define PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
#else
#define PULSAR_ELSE_CHECK_BAD_UNIFORM(handle) else\
	Logger::LogWarning("Failed to set uniform for shader at handle (" + std::to_string(handle) + ").");
#endif
#endif // PULSAR_ELSE_CHECK_BAD_UNIFORM


ShaderHandle ShaderRegistry::handle_cap;
std::unordered_map<ShaderHandle, Shader> ShaderRegistry::registry;
std::unordered_map<ShaderConstructArgs, ShaderHandle> ShaderRegistry::lookup_map;
ShaderHandle ShaderRegistry::standard_shader;

Shader const* ShaderRegistry::Get(ShaderHandle handle)
{
	auto iter = registry.find(handle);
	if (iter != registry.end())
		return &iter->second;
	else
	{
#if !PULSAR_IGNORE_WARNINGS_NULL_SHADER
		Logger::LogWarning("Shader handle (" + std::to_string(handle) + ") does not exist in ShaderRegistry.");
#endif
		return nullptr;
	}
}

bool ShaderRegistry::DestroyShader(ShaderHandle handle)
{
	auto iter = registry.find(handle);
	if (iter == registry.end())
		return false;
	registry.erase(iter);
	return true;
}

void ShaderRegistry::Init()
{
	handle_cap = 1;
	auto status = Loader::loadShader(_PulsarSettings::standard_shader_assetfile.c_str(), standard_shader);
	if (status != LOAD_STATUS::OK)
		Logger::LogErrorFatal("Standard shader could not be loaded (error code " + std::to_string(static_cast<int>(status)) + "): " + _PulsarSettings::standard_shader_assetfile);
}

void ShaderRegistry::Terminate()
{
	registry.clear();
	lookup_map.clear();
}

ShaderHandle ShaderRegistry::GetHandle(const ShaderConstructArgs& args)
{
	auto iter = lookup_map.find(args);
	if (iter != lookup_map.end())
		return iter->second;
	Shader shader(args.vertexShader.c_str(), args.fragmentShader.c_str());
	if (shader.IsValid())
	{
		ShaderHandle handle = handle_cap++;
		registry.emplace(handle, std::move(shader));
		lookup_map[args] = handle;
		return handle;
	}
	else return 0;
}

void ShaderRegistry::Bind(ShaderHandle handle)
{
	Shader const* shader = Get(handle);
	if (shader)
		shader->Bind();
#if !PULSAR_IGNORE_WARNINGS_NULL_SHADER
	else
		Logger::LogWarning("Failed to bind shader at handle (" + std::to_string(handle) + ").");
#endif
}

void ShaderRegistry::Unbind()
{
	PULSAR_TRY(glUseProgram(0));
}

void ShaderRegistry::SetUniform1i(ShaderHandle handle, const char* uniform_name, const GLint value)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniform1i(shader->GetUniformLocation(uniform_name), value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniform2iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniform2iv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniform3iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniform3iv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniform4iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniform4iv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniform1ui(ShaderHandle handle, const char* uniform_name, const GLuint value)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniform1ui(shader->GetUniformLocation(uniform_name), value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniform2uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniform2uiv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniform3uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniform3uiv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniform4uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniform4uiv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniform1f(ShaderHandle handle, const char* uniform_name, const GLfloat value)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniform1f(shader->GetUniformLocation(uniform_name), value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniform2fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniform2fv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniform3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniform3fv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniform4fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniform4fv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniformMatrix2fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniformMatrix2fv(shader->GetUniformLocation(uniform_name), array_count, GL_FALSE, value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniformMatrix3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniformMatrix3fv(shader->GetUniformLocation(uniform_name), array_count, GL_FALSE, value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderRegistry::SetUniformMatrix4fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		PULSAR_TRY(glUniformMatrix4fv(shader->GetUniformLocation(uniform_name), array_count, GL_FALSE, value));
	}
	PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
}
