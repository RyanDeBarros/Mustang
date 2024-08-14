#pragma once

#include <glm/glm.hpp>

struct Transform2D
{
	glm::vec2 position = { 0.0f, 0.0f };
	glm::float32 rotation = 0.0f;
	glm::vec2 scale = { 1.0f, 1.0f };

	Transform2D operator^(const Transform2D& transform) const;

	inline bool operator==(const Transform2D& other) const { return position == other.position && rotation == other.rotation && scale == other.scale; }
};

struct PackedTransform2D;

namespace Transform
{
	extern glm::mat3 ToMatrix(const Transform2D& tr);
	extern glm::mat3 ToInverseMatrix(const Transform2D& tr);
	extern glm::mat3x2 ToCondensedMatrix(const Transform2D& tr);
	extern glm::mat2 CondensedRS(const Transform2D& tr);
	extern Transform2D Inverse(const Transform2D& tr);
	extern glm::mat2 Rotation(glm::float32 r);
	extern Transform2D RelTo(const Transform2D& global, const Transform2D& parent);
	extern Transform2D AbsTo(const Transform2D& local, const Transform2D& parent);

	extern glm::vec4 Gamma(const Transform2D& tr);
	extern glm::vec2 CondensedP(const glm::vec2& position, const PackedTransform2D& parent);
	extern glm::mat2 CondensedRS(const glm::vec4& gamma, const glm::vec4& parent_gamma);
	extern glm::vec4 CombineGamma(const glm::vec4& gamma, const glm::vec4& parent_gamma);
}

struct PackedTransform2D
{
	glm::vec2 position = { 0.0f, 0.0f };
	glm::vec4 gamma = { 1.0f, 0.0f, 1.0f, 0.0f };

	PackedTransform2D(const glm::vec2& position, const glm::vec4& gamma) : position(position), gamma(gamma) {}
	PackedTransform2D(const Transform2D& transform = {}) : position(transform.position), gamma(Transform::Gamma(transform)) {}
	void Set(const Transform2D& transform = {}) { position = transform.position; gamma = Transform::Gamma(transform); }
};
