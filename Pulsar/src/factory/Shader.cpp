#include "Shader.h"

#include <string>
#include <iostream>
#include <GL/glew.h>

#include "Macros.h"

char* read_file(const char* filepath)
{
	FILE* file;
	fopen_s(&file, filepath, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		const long filesize = ftell(file);
		fseek(file, 0, SEEK_SET);

		char* buffer = new char[filesize + 1];
		fread(buffer, 1, filesize + 1, file);
		buffer[filesize] = '\0';
		return buffer;
	}
	else
	{
		Logger::LogError(std::string("Could not read file \"") + filepath + "\"");
		return nullptr;
	}
}

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
	: m_RID(0), m_VertexFilepath(vertex_filepath), m_FragmentFilepath(fragment_filepath)
{
	const char* vertex_shader = read_file(vertex_filepath);
	const char* fragment_shader = read_file(fragment_filepath);
	if (vertex_shader && fragment_shader)
	{
		TRY(m_RID = glCreateProgram());
		GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader, vertex_filepath);
		if (vs == 0)
		{
			TRY(glDeleteProgram(m_RID));
			m_RID = 0;
		}
		else
		{
			GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader, fragment_filepath);
			if (fs == 0)
			{
				TRY(glDeleteProgram(m_RID));
				TRY(glDeleteShader(vs));
				m_RID = 0;
			}
			else
			{
				TRY(glAttachShader(m_RID, vs));
				TRY(glAttachShader(m_RID, fs));
				TRY(glLinkProgram(m_RID));
				TRY(glValidateProgram(m_RID));

				TRY(glDeleteShader(vs));
				TRY(glDeleteShader(fs));
			}
		}
	}
	delete[] vertex_shader;
	delete[] fragment_shader;
}

Shader::Shader(Shader&& shader) noexcept
	: m_RID(shader.m_RID), m_VertexFilepath(shader.m_VertexFilepath), m_FragmentFilepath(shader.m_FragmentFilepath)
{
	shader.m_RID = 0;
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
		Logger::LogWarning(std::string("No uniform exists or is in use under the name: ") + uniform_name);
	m_UniformLocationCache[uniform_name] = location;
	return location;
}
