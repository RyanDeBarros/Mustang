#pragma once

#include <unordered_map>
#include <GL/glew.h>

#include <string>

#include "Typedefs.h"

class Shader
{
	RID m_RID;
	const std::string m_VertexFilepath, m_FragmentFilepath;
	mutable std::unordered_map<const char*, GLint> m_UniformLocationCache;
public:
	Shader(const char* vertex_filepath, const char* fragment_filepath);
	Shader(Shader&& shader) noexcept;
	Shader(const Shader& shader) = delete;
	~Shader();

	void Bind() const;
	void Unbind() const;

	GLint GetUniformLocation(const char* uniform_name) const;
	
	inline bool Equivalent(const char* vertex_filepath, const char* fragment_filepath) const { return m_VertexFilepath == vertex_filepath && m_FragmentFilepath == fragment_filepath; }
	inline bool IsValid() const { return m_RID > 0; }
};
