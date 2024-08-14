#include "Transform.h"

#include "utils/CommonMath.h"

namespace Transform {
	
	glm::mat3 ToMatrix(const Transform2D& tr)
	{
		return glm::mat3(tr.scale.x * cosf(tr.rotation), tr.scale.x * sinf(tr.rotation), 0, -tr.scale.y * sinf(tr.rotation), tr.scale.y * cosf(tr.rotation), 0, tr.position.x, tr.position.y, 1);
	}

	glm::mat3 ToInverseMatrix(const Transform2D& tr)
	{
		return glm::mat3(cosf(tr.rotation) / tr.scale.x, -sinf(tr.rotation) / tr.scale.x, 0, sinf(tr.rotation) / tr.scale.y, cosf(tr.rotation) / tr.scale.y, 0, -tr.position.x, -tr.position.y, 1);
	}

	glm::mat3x2 ToCondensedMatrix(const Transform2D& tr)
	{
		return glm::mat3x2(tr.position.x, tr.position.y, tr.scale.x * cosf(tr.rotation), tr.scale.x * sinf(tr.rotation), -tr.scale.y * sinf(tr.rotation), tr.scale.y * cosf(tr.rotation));
	}

	glm::mat2 CondensedRS(const Transform2D& tr)
	{
		return glm::mat2(tr.scale.x * cosf(tr.rotation), tr.scale.x * sinf(tr.rotation), -tr.scale.y * sinf(tr.rotation), tr.scale.y * cosf(tr.rotation));
	}
	
	Transform2D Inverse(const Transform2D& tr)
	{
		return { -tr.position, -tr.rotation, DivideOrZero(1.0f, tr.scale) };
	}

	glm::mat2 Rotation(glm::float32 r)
	{
		return { glm::cos(r), glm::sin(r), -glm::sin(r), glm::cos(r) };
	}

	Transform2D RelTo(const Transform2D& global, const Transform2D& parent)
	{
		return { (Transform::Rotation(-parent.rotation) * (global.position - parent.position)) / parent.scale, global.rotation - parent.rotation, global.scale / parent.scale };
	}

	Transform2D AbsTo(const Transform2D& local, const Transform2D& parent)
	{
		return { parent.position + Transform::Rotation(parent.rotation) * (parent.scale * local.position), parent.rotation + local.rotation, parent.scale * local.scale };
	}

	glm::vec4 Gamma(const Transform2D& tr)
	{
		return { tr.scale.x * glm::cos(tr.rotation), tr.scale.x * glm::sin(tr.rotation), tr.scale.y * glm::cos(tr.rotation), tr.scale.y * glm::sin(tr.rotation) };
	}

	glm::vec2 CondensedP(const glm::vec2& position, const PackedTransform2D& parent)
	{
		return { parent.position.x + parent.gamma[0] * position.x - parent.gamma[3] * position.y, parent.position.y + parent.gamma[1] * position.x + parent.gamma[2] * position.y };
	}

	glm::mat2 CondensedRS(const glm::vec4& gamma, const glm::vec4& parent_gamma)
	{
		return { parent_gamma[0] * gamma[0] - parent_gamma[3] * gamma[1], parent_gamma[1] * gamma[0] + parent_gamma[2] * gamma[1], -parent_gamma[0] * gamma[3] - parent_gamma[3] * gamma[2], -parent_gamma[1] * gamma[3] + parent_gamma[2] * gamma[2] };
	}

	glm::vec4 CombineGamma(const glm::vec4& gamma, const glm::vec4& parent_gamma)
	{
		return { parent_gamma[0] * gamma[0] - parent_gamma[3] * gamma[1], parent_gamma[1] * gamma[0] + parent_gamma[2] * gamma[1], parent_gamma[0] * gamma[3] + parent_gamma[3] * gamma[2], -parent_gamma[1] * gamma[3] + parent_gamma[2] * gamma[2] };
	}

}

Transform2D Transform2D::operator^(const Transform2D& transform) const
{
	return { position + transform.position, rotation + transform.rotation, scale * transform.scale };
}
