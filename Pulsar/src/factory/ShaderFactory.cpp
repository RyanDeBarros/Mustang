#include "ShaderFactory.h"

#include "Macros.h"
#include "Logger.h"
#include "AssetLoader.h"
#include "RendererSettings.h"

ShaderHandle ShaderFactory::handle_cap;
std::unordered_map<ShaderHandle, Shader*> ShaderFactory::factory;
ShaderHandle ShaderFactory::standard_shader;

Shader* ShaderFactory::Get(ShaderHandle handle)
{
	auto iter = factory.find(handle);
	if (iter != factory.end())
		return iter->second;
	else
	{
		Logger::LogWarning("Shader handle (" + std::to_string(handle) + ") does not exist in ShaderFactory.");
		return nullptr;
	}
}

void ShaderFactory::Init()
{
	handle_cap = 1;
	auto status = loadShader(_RendererSettings::standard_shader_assetfile.c_str(), standard_shader);
	if (status != LOAD_STATUS::OK)
		Logger::LogErrorFatal("Standard shader could not be loaded (error code " + std::to_string(static_cast<int>(status)) + "): " + _RendererSettings::standard_shader_assetfile);
}

void ShaderFactory::Terminate()
{
	for (const auto& [handle, shader] : factory)
		delete shader;
	factory.clear();
}

ShaderHandle ShaderFactory::GetHandle(const char* vertex_shader, const char* fragment_shader)
{
	for (const auto& [handle, shader] : factory)
	{
		if (shader->Equivalent(vertex_shader, fragment_shader))
			return handle;
	}
	Shader shader(vertex_shader, fragment_shader);
	if (shader.IsValid())
	{
		ShaderHandle handle = handle_cap++;
		factory.emplace(handle, new Shader(std::move(shader)));
		return handle;
	}
	else return 0;
}

void ShaderFactory::Bind(ShaderHandle handle)
{
	Shader* shader = Get(handle);
	if (shader)
		shader->Bind();
	else
		Logger::LogWarning("Failed to bind shader at handle (" + std::to_string(handle) + ").");
}

void ShaderFactory::Unbind()
{
	TRY(glUseProgram(0));
}

void bad_uniform(ShaderHandle handle)
{
	Logger::LogWarning("Failed to set uniform for shader at handle (" + std::to_string(handle) + ").");
}

void ShaderFactory::SetUniform1i(ShaderHandle handle, const char* uniform_name, const GLint value)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform1i(shader->GetUniformLocation(uniform_name), value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniform2iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform2iv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniform3iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform3iv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniform4iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform4iv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniform1ui(ShaderHandle handle, const char* uniform_name, const GLuint value)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform1ui(shader->GetUniformLocation(uniform_name), value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniform2uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform2uiv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniform3uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform3uiv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniform4uiv(ShaderHandle handle, const char* uniform_name, const GLuint* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform4uiv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniform1f(ShaderHandle handle, const char* uniform_name, const GLfloat value)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform1f(shader->GetUniformLocation(uniform_name), value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniform2fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform2fv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniform3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform3fv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniform4fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform4fv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniformMatrix2fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniformMatrix2fv(shader->GetUniformLocation(uniform_name), array_count, GL_FALSE, value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniformMatrix3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniformMatrix3fv(shader->GetUniformLocation(uniform_name), array_count, GL_FALSE, value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniformMatrix4fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniformMatrix4fv(shader->GetUniformLocation(uniform_name), array_count, GL_FALSE, value));
	}
	else
		bad_uniform(handle);
}
