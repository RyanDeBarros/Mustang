#pragma once

#include <unordered_map>
#include <GL/glew.h>

#include <string>

#include "Typedefs.h"

class Shader
{
	RID m_RID;
	const std::string vertex, fragment;
	mutable std::unordered_map<const char*, GLint> m_UniformLocationCache;
public:
	Shader(const char* vertex_filepath, const char* fragment_filepath);
	Shader(Shader&& shader) noexcept;
	Shader(const Shader& shader) = delete;
	~Shader();

	void Bind() const;
	void Unbind() const;

	GLint GetUniformLocation(const char* uniform_name) const;
	
	inline bool equivalent(const char* vertex_filepath, const char* fragment_filepath) const { return vertex == vertex_filepath && fragment == fragment_filepath; }
	inline bool IsValid() const { return m_RID > 0; }
};
