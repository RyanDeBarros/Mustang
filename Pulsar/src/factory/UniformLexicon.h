#pragma once

#include <string>
#include <map>
#include <variant>
#include <algorithm>

#include <GL/glew.h>
#include <glm/glm.hpp>

typedef std::variant<
	GLint, glm::ivec2, glm::ivec3, glm::ivec4,
	GLuint, glm::uvec2, glm::uvec3, glm::uvec4,
	GLfloat, glm::vec2, glm::vec3, glm::vec4,
	glm::mat2, glm::mat3, glm::mat4
> Uniform;

class UniformLexicon
{
	friend class UniformLexiconFactory;
	std::map<std::string, Uniform> m_Uniforms;
	inline bool Equivalent(const std::map<std::string, Uniform>& uniforms) const { return m_Uniforms == uniforms; }

public:
	UniformLexicon(const std::map<std::string, Uniform>& uniforms);
	~UniformLexicon();

	void MergeLexicon(const UniformLexicon& lexicon);
	bool Shares(const UniformLexicon& lexicon);
};
