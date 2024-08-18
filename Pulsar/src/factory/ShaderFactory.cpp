#include "ShaderFactory.h"

#include "Macros.h"
#include "Logger.inl"
#include "AssetLoader.h"
#include "RendererSettings.h"

#if PULSAR_IGNORE_WARNINGS_NULL_SHADER
#define ELSE_CHECK_BAD_UNIFORM(handle)
#else
#define ELSE_CHECK_BAD_UNIFORM(handle) else\
	Logger::LogWarning("Failed to set uniform for shader at handle (" + std::to_string(handle) + ").");
#endif


ShaderHandle ShaderFactory::handle_cap;
std::unordered_map<ShaderHandle, Shader*> ShaderFactory::factory;
std::unordered_map<ShaderConstructArgs, ShaderHandle> ShaderFactory::lookup_map;
ShaderHandle ShaderFactory::standard_shader;

Shader const* ShaderFactory::Get(ShaderHandle handle)
{
	auto iter = factory.find(handle);
	if (iter != factory.end())
		return iter->second;
	else
	{
#if !PULSAR_IGNORE_WARNINGS_NULL_SHADER
		Logger::LogWarning("Shader handle (" + std::to_string(handle) + ") does not exist in ShaderFactory.");
#endif
		return nullptr;
	}
}

void ShaderFactory::Init()
{
	handle_cap = 1;
	auto status = Loader::loadShader(_RendererSettings::standard_shader_assetfile.c_str(), standard_shader);
	if (status != LOAD_STATUS::OK)
		Logger::LogErrorFatal("Standard shader could not be loaded (error code " + std::to_string(static_cast<int>(status)) + "): " + _RendererSettings::standard_shader_assetfile);
}

void ShaderFactory::Terminate()
{
	for (const auto& [handle, shader] : factory)
	{
		if (shader)
			delete shader;
	}
	factory.clear();
	lookup_map.clear();
}

ShaderHandle ShaderFactory::GetHandle(const ShaderConstructArgs& args)
{
	auto iter = lookup_map.find(args);
	if (iter != lookup_map.end())
		return iter->second;
	Shader shader(args.vertexShader.c_str(), args.fragmentShader.c_str());
	if (shader.IsValid())
	{
		ShaderHandle handle = handle_cap++;
		factory.emplace(handle, new Shader(std::move(shader)));
		lookup_map[args] = handle;
		return handle;
	}
	else return 0;
}

void ShaderFactory::Bind(ShaderHandle handle)
{
	Shader const* shader = Get(handle);
	if (shader)
		shader->Bind();
#if !PULSAR_IGNORE_WARNINGS_NULL_SHADER
	else
		Logger::LogWarning("Failed to bind shader at handle (" + std::to_string(handle) + ").");
#endif
}

void ShaderFactory::Unbind()
{
	TRY(glUseProgram(0));
}

void ShaderFactory::SetUniform1i(ShaderHandle handle, const char* uniform_name, const GLint value)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform1i(shader->GetUniformLocation(uniform_name), value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniform2iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform2iv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniform3iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform3iv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniform4iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform4iv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniform1ui(ShaderHandle handle, const char* uniform_name, const GLuint value)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform1ui(shader->GetUniformLocation(uniform_name), value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniform2uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform2uiv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniform3uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform3uiv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniform4uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform4uiv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniform1f(ShaderHandle handle, const char* uniform_name, const GLfloat value)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform1f(shader->GetUniformLocation(uniform_name), value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniform2fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform2fv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniform3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform3fv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniform4fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform4fv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniformMatrix2fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniformMatrix2fv(shader->GetUniformLocation(uniform_name), array_count, GL_FALSE, value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniformMatrix3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniformMatrix3fv(shader->GetUniformLocation(uniform_name), array_count, GL_FALSE, value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}

void ShaderFactory::SetUniformMatrix4fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader const* shader = Get(handle);
	if (shader)
	{
		TRY(glUniformMatrix4fv(shader->GetUniformLocation(uniform_name), array_count, GL_FALSE, value));
	}
	ELSE_CHECK_BAD_UNIFORM(handle)
}
