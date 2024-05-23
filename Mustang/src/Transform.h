#pragma once

#include "glm/glm.hpp"

static inline glm::vec2 operator/(const float& f, const glm::vec2& v)
{
	return glm::vec2(1 / v.x, 1 / v.y);
}

struct Transform2D
{
	glm::vec2 position = glm::vec2(0.0f, 0.0f);
	glm::float32 rotation = 0.0f;
	glm::vec2 scale = glm::vec2(1.0f, 1.0f);
};

namespace Transform
{
	inline glm::mat3 ToMatrix(const Transform2D& tr)
	{
		return glm::mat3(tr.scale.x * cosf(tr.rotation), tr.scale.x * sinf(tr.rotation), 0, -tr.scale.y * sinf(tr.rotation), tr.scale.y * cosf(tr.rotation), 0, tr.position.x, tr.position.y, 1);
	}

	inline Transform2D Inverse(const Transform2D& tr)
	{
		return {-tr.position, -tr.rotation, 1 / tr.scale};
	}
}
