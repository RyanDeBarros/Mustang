#pragma once


#include <unordered_map>
#include <GL/glew.h>

#include "Typedefs.h"

class Shader
{
	RID m_RID;
	mutable std::unordered_map<const char*, GLint> m_UniformLocationCache;
public:
	Shader(const char* vertex_filepath, const char* fragment_filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	GLint GetUniformLocation(const char* uniform_name) const;
};
