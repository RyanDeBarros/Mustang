#pragma once

#include <glm/glm.hpp>

struct Transform2D
{
	glm::vec2 position = glm::vec2(0.0f, 0.0f);
	glm::float32 rotation = 0.0f;
	glm::vec2 scale = glm::vec2(1.0f, 1.0f);

	Transform2D operator^(const Transform2D& transform) const;
};

namespace Transform
{

	extern inline glm::mat3 ToMatrix(const Transform2D& tr);
	extern inline glm::mat3 ToInverseMatrix(const Transform2D& tr);
	extern inline glm::mat3x2 ToCondensedMatrix(const Transform2D& tr);
	extern inline Transform2D Inverse(const Transform2D& tr);

}
