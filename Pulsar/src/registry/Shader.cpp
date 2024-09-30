#include "Shader.h"

#include <GL/glew.h>

#include <string>
#include <iostream>

#include "Macros.h"
#include "Logger.inl"
#include "IO.h"
#include "PulsarSettings.h"
#include "AssetLoader.h"

static GLuint compile_shader(GLenum type, const char* shader, const char*filepath)
{
	PULSAR_TRY(GLuint id = glCreateShader(type));
	PULSAR_TRY(glShaderSource(id, 1, &shader, nullptr));
	PULSAR_TRY(glCompileShader(id));

	int result;
	PULSAR_TRY(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		PULSAR_TRY(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		GLchar* message = new GLchar[length];
		PULSAR_TRY(glGetShaderInfoLog(id, length, &length, message));
		Logger::LogError(std::string("Failed to compile ") + (type == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader: \"" + filepath + "\"\n" + message);
		delete[] message;
		PULSAR_TRY(glDeleteShader(id));
		return 0;
	}
	return id;
}

Shader::Shader(const ShaderConstructArgs& args)
	: m_RID(0)
{
	std::string vertex_shader;
	if (!IO::read_file(args.vertexFilepath.c_str(), vertex_shader))
		Logger::LogError("Could not read vertex shader: \"" + args.vertexFilepath + "\"");
	std::string fragment_shader;
	if (!IO::read_file(args.fragmentFilepath.c_str(), fragment_shader))
		Logger::LogError("Could not read fragment shader: \"" + args.fragmentFilepath + "\"");
	
	if (!vertex_shader.empty() && !fragment_shader.empty())
	{
		PULSAR_TRY(m_RID = glCreateProgram());
		GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader.c_str(), args.vertexFilepath.c_str());
		if (vs == 0)
		{
			PULSAR_TRY(glDeleteProgram(m_RID));
			m_RID = 0;
		}
		else
		{
			GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader.c_str(), args.fragmentFilepath.c_str());
			if (fs == 0)
			{
				PULSAR_TRY(glDeleteProgram(m_RID));
				PULSAR_TRY(glDeleteShader(vs));
				m_RID = 0;
			}
			else
			{
				PULSAR_TRY(glAttachShader(m_RID, vs));
				PULSAR_TRY(glAttachShader(m_RID, fs));
				PULSAR_TRY(glLinkProgram(m_RID));
				PULSAR_TRY(glValidateProgram(m_RID));

				PULSAR_TRY(glDeleteShader(vs));
				PULSAR_TRY(glDeleteShader(fs));
			}
		}
	}
}

Shader::Shader(Shader&& shader) noexcept
	: m_RID(shader.m_RID)
{
	shader.m_RID = 0;
}

Shader& Shader::operator=(Shader&& shader) noexcept
{
	if (this == &shader)
		return *this;
	if (m_RID != shader.m_RID)
	{
		PULSAR_TRY(glDeleteProgram(m_RID));
	}
	m_RID = shader.m_RID;
	shader.m_RID = 0;
	return *this;
}

Shader::~Shader()
{
	PULSAR_TRY(glDeleteProgram(m_RID));
	m_RID = 0;
}

void Shader::Bind() const
{
	PULSAR_TRY(glUseProgram(m_RID));
}

void Shader::Unbind() const
{
	PULSAR_TRY(glUseProgram(0));
}

GLint Shader::GetUniformLocation(const char* uniform_name) const
{
	if (m_UniformLocationCache.find(uniform_name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[uniform_name];
	PULSAR_TRY(GLint location = glGetUniformLocation(m_RID, uniform_name));
#if !PULSAR_IGNORE_WARNINGS_NULL_SHADER
	if (location == -1)
		Logger::LogWarning(std::string("No uniform exists or is in use under the name: ") + uniform_name);
#endif
	m_UniformLocationCache[uniform_name] = location;
	return location;
}

#ifndef PULSAR_ELSE_CHECK_BAD_UNIFORM
#if PULSAR_IGNORE_WARNINGS_NULL_SHADER
#define PULSAR_ELSE_CHECK_BAD_UNIFORM(handle)
#else
#define PULSAR_ELSE_CHECK_BAD_UNIFORM(handle) else\
	Logger::LogWarning("Failed to set uniform for shader at handle (" + std::to_string(handle) + ").");
#endif
#endif // PULSAR_ELSE_CHECK_BAD_UNIFORM

void ShaderRegistry::DefineStandardShader()
{
	auto status = Loader::loadShader(PulsarSettings::standard_shader_assetfile(), standard_shader);
	if (status != LOAD_STATUS::OK)
		Logger::LogErrorFatal("Standard shader could not be loaded (error code " + std::to_string(static_cast<int>(status)) + "): " + PulsarSettings::standard_shader_assetfile());
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
