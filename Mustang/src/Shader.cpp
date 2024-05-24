#include "Shader.h"

#include <string>
#include <iostream>
#include <GL/glew.h>

#include "Utility.h"

static GLuint compile_shader(GLenum type, const char* shader, const char*filepath)
{
	TRY(
		GLuint id = glCreateShader(type);
		glShaderSource(id, 1, &shader, nullptr);
		glCompileShader(id);
	)

	int result;
	TRY(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		TRY(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		GLchar* message = new GLchar[length];
		TRY(glGetShaderInfoLog(id, length, &length, message));
		Logger::LogError(std::string("Failed to compile ") + (type == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader: \"" + filepath + "\"\n" + message);
		delete[] message;
		TRY(glDeleteShader(id));
		return 0;
	}
	return id;
}

Shader::Shader(const char* vertex_filepath, const char* fragment_filepath)
	: m_RID(0)
{
	const char* vertex_shader = Utility::ReadFile(vertex_filepath);
	const char* fragment_shader = Utility::ReadFile(fragment_filepath);
	if (vertex_shader && fragment_shader)
	{
		TRY(m_RID = glCreateProgram());
		GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader, vertex_filepath);
		GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader, fragment_filepath);
		if (vs != 0 && fs != 0)
		{
			TRY(
				glAttachShader(m_RID, vs);
				glAttachShader(m_RID, fs);
				glLinkProgram(m_RID);
				glValidateProgram(m_RID);
			)
			TRY(
				glDeleteShader(vs);
				glDeleteShader(fs);
			)
		}
		else
			m_RID = 0;
	}
	delete[] vertex_shader;
	delete[] fragment_shader;
}

Shader::~Shader()
{
	TRY(glDeleteProgram(m_RID));
}

void Shader::Bind() const
{
	TRY(glUseProgram(m_RID));
}

void Shader::Unbind() const
{
	TRY(glUseProgram(0));
}

GLint Shader::GetUniformLocation(const char* uniform_name) const
{
	if (m_UniformLocationCache.find(uniform_name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[uniform_name];
	TRY(GLint location = glGetUniformLocation(m_RID, uniform_name));
	if (location == -1)
		Logger::LogWarning(std::string("No uniform exists under the name: ") + uniform_name);
	m_UniformLocationCache[uniform_name] = location;
	return location;
}
