#pragma once

#include <glm/glm.hpp>

struct Transform2D
{
	glm::vec2 position = glm::vec2(0.0f, 0.0f);
	glm::float32 rotation = 0.0f;
	glm::vec2 scale = glm::vec2(1.0f, 1.0f);

	Transform2D operator^(const Transform2D& transform) const;

	inline bool operator==(const Transform2D& other) const { return position == other.position && rotation == other.rotation && scale == other.scale; }
};

namespace Transform
{
	extern glm::mat3 ToMatrix(const Transform2D& tr);
	extern glm::mat3 ToInverseMatrix(const Transform2D& tr);
	extern glm::mat3x2 ToCondensedMatrix(const Transform2D& tr);
	extern glm::mat2 CondensedRS(const Transform2D& tr);
	extern Transform2D Inverse(const Transform2D& tr);
	extern glm::mat2 Rotation(const glm::float32& r);
	extern Transform2D RelTo(const Transform2D& global, const Transform2D& parent);
	extern Transform2D AbsTo(const Transform2D& local, const Transform2D& parent);
}
