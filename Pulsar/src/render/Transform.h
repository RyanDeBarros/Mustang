#pragma once

#include <glm/glm.hpp>
#include <functional>

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

class Transformable2D
{
public:
	virtual Transform2D GetTransform() const = 0;
	virtual void SetTransform(const Transform2D&) = 0;
	virtual void OperatePosition(const std::function<void(glm::vec2& position)>&) = 0;
	virtual void OperateRotation(const std::function<void(glm::float32& rotation)>&) = 0;
	virtual void OperateScale(const std::function<void(glm::vec2& scale)>&) = 0;
};
