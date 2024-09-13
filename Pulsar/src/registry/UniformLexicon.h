#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <algorithm>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Handles.inl"

typedef std::variant<
	GLint, glm::ivec2, glm::ivec3, glm::ivec4,
	GLuint, glm::uvec2, glm::uvec3, glm::uvec4,
	GLfloat, glm::vec2, glm::vec3, glm::vec4,
	glm::mat2, glm::mat3, glm::mat4
> Uniform;

// TODO don't use class for this. Just use unordered_map, and define utility functions like Shares, MergeLexicon, GetValue, etc.
class UniformLexicon
{
	friend class UniformLexiconRegistry;
	std::unordered_map<std::string, Uniform> m_Uniforms;

public:
	UniformLexicon(const std::unordered_map<std::string, Uniform>& uniforms = {});
	UniformLexicon(const UniformLexicon&);
	UniformLexicon(UniformLexicon&&) noexcept;
	UniformLexicon& operator=(const UniformLexicon&);
	UniformLexicon& operator=(UniformLexicon&&) noexcept;
	~UniformLexicon();

	void MergeLexicon(UniformLexiconHandle lexicon_handle);
	bool Shares(const UniformLexicon& lexicon);
	bool Shares(UniformLexiconHandle lexicon_handle);
	void OnApply(ShaderHandle shader) const;

private:
	Uniform const* GetValue(const std::string& name) const;
	bool SetValue(const std::string& name, const Uniform& uniform);
	bool DefineNewValue(const std::string& name, const Uniform& uniform);

	friend class CanvasLayer;
	void Clear();
};
