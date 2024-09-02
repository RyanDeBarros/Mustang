#pragma once

#include <GL/glew.h>

#include <unordered_map>
#include <string>

typedef GLuint Shader_RID;

class Shader
{
	Shader_RID m_RID;
	mutable std::unordered_map<const char*, GLint> m_UniformLocationCache;

public:
	Shader(const char* vertex_filepath, const char* fragment_filepath);
	Shader(const Shader& shader) = delete;
	Shader(Shader&& shader) noexcept;
	Shader& operator=(Shader&& shader) noexcept;
	~Shader();

	void Bind() const;
	void Unbind() const;

	GLint GetUniformLocation(const char* uniform_name) const;
	
	bool IsValid() const { return m_RID > 0; }
};
