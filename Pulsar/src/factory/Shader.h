#pragma once

#include <GL/glew.h>

#include <unordered_map>
#include <string>

#include "Typedefs.h"

class Shader
{
	RID m_RID;
	std::string m_VertexFilepath, m_FragmentFilepath;
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
	
	inline bool Equivalent(const char* vertex_filepath, const char* fragment_filepath) const { return m_VertexFilepath == vertex_filepath && m_FragmentFilepath == fragment_filepath; }
	inline bool IsValid() const { return m_RID > 0; }
};
