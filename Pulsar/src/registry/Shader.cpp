#include "Shader.h"

#include <GL/glew.h>

#include <string>
#include <iostream>

#include "Macros.h"
#include "Logger.inl"
#include "IO.h"

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

Shader::Shader(const char* vertex_filepath, const char* fragment_filepath)
	: m_RID(0)
{
	std::string vertex_shader;
	if (!IO::read_file(vertex_filepath, vertex_shader))
		Logger::LogError(std::string("Could not read vertex shader: \"") + vertex_filepath + "\"");
	std::string fragment_shader;
	if (!IO::read_file(fragment_filepath, fragment_shader))
		Logger::LogError(std::string("Could not read fragment shader: \"") + fragment_filepath + "\"");
	
	if (!vertex_shader.empty() && !fragment_shader.empty())
	{
		PULSAR_TRY(m_RID = glCreateProgram());
		GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader.c_str(), vertex_filepath);
		if (vs == 0)
		{
			PULSAR_TRY(glDeleteProgram(m_RID));
			m_RID = 0;
		}
		else
		{
			GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader.c_str(), fragment_filepath);
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
