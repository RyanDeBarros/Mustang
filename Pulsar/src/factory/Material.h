#pragma once

#include <string>
#include <vector>
#include <variant>

#include <GL/glew.h>
#include <glm/glm.hpp>

typedef std::variant<GLint, GLuint, GLfloat, glm::ivec2, glm::ivec3, glm::ivec4, glm::uvec2, glm::uvec3, glm::uvec4, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> MaterialUniformType;

struct MaterialUniform
{
	std::string name;
	MaterialUniformType data;

	inline bool operator==(const MaterialUniform& uniform) const { return name == uniform.name && data == uniform.data; }
};

class Material
{
	friend class MaterialFactory;
	std::vector<MaterialUniform> m_Uniforms;
	inline bool equivalent(const std::vector<MaterialUniform>& uniforms) const { return m_Uniforms == uniforms; }

public:
	Material(const std::vector<MaterialUniform>& uniform_vector);
	~Material();
};
